#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <atomic>

#include <Util/Assert.h>
#include <Util/Macro.h>

#include <Memory/BaseSchema.h>

#include <Parallel/SpinLock.h>

#include <EASTL/unique_ptr.h>
#include <EASTL/tuple.h>

namespace Foundation
{
namespace Memory
{
class BaseAllocator;

template <typename t_elementType, size_t t_pageCount>
class PoolSchema : public BaseSchema
{
   class Page
   {
    public:
      void* GetMemoryAddress(uint32_t p_index)
      {
         t_elementType* freeObject = &currentPage->m_objects[p_index];
         return static_cast<void*>(freeObject);
      }

      // Returns the address of a reused element
      void* GetReusedElement()
      {
         // The free index is invalid. This means that there are no free elements available
         if (m_freeElementIndex == InvalidElementIndex)
         {
            return nullptr;
         }

         // Get the address of the index that is free
         const uint32_t nextIndex = m_nextIndex[m_freeElementIndex];
         ASSERT(nextIndex != InvalidElementIndex, "This index is invalid, but it shouldn't be");

         // Get the memory of the reused element
         void* objectAddress = GetMemoryAddress(m_freeElementIndex);
         m_nextIndex[m_freeElementIndex] = InvalidElementIndex;

         // Set the next free index
         m_freeElementIndex = nextIndex;

         // Increment the element count
         m_elementCount++;
      }

      // Allocate an element that hasn't been used yet
      inline void* AllocateElement()
      {
         if (m_elementIndex < t_pageCount)
         {
            // increment the element index
            const uint32_t index = m_elementIndex++;

            objectAddress = currentPage->GetMemoryAddress(index);

            // Increment the total amount of allocated elements
            m_elementCount++;

            return objectAddress;
         }

         return nullptr;
      }

      inline bool FreeElement(uint32_t p_index)
      {
         ASSERT(m_nextIndex[p_index] == InvalidElementIndex, "This index should be invalid");

         m_nextIndex[p_index] = m_freeElementIndex;
         m_freeElementIndex = p_index;
         m_elementCount--;
      }

      // Checks if the page is already full
      inline bool IsFull()
      {
         return m_elementCount >= t_pageCount;
      }

      inline bool IsEmpty()
      {
         return m_elementCount == 0u;
      }

      // TODO: make this a constexpr
      auto GetAllocatedMemoryRange()
      {
         const uint64_t start = static_cast<uint64_t>(&m_objects[0u]);
         const uint64_t end = static_cast<uint64_t>(&m_objects[t_pageCount - 1u]);
         ASSERT(end > start, "The allocated page range isn't correct");
         return eastl::make_tuple<uint64_t, uint64_t>(start, end);
      }

    private:
      // Array of objects
      t_elementType m_objects[t_pageCount] = {};
      // List of elements that link to the next free index
      uint32_t m_nextIndex[t_pageCount] = {InvalidElementIndex};

      // Current element index
      uint32_t m_elementIndex = 0u;
      // Total allocated elements in the page
      uint32_t m_elementCount = 0u;
      // The first free index
      uint32_t m_freeElementIndex = InvalidElementIndex;

      // Pointer to next page
      Page* m_next = nullptr;
   };

   const uint32_t InvalidElementIndex = static_cast<uint32_t>(-1);

   using PoolSchemaType = PoolSchema<t_elementType, t_pageCount>;

 public:
   static eastl::unique_ptr<BaseSchema> CreateSchema(const BaseSchema::Descriptor& p_desc, BaseAllocator* p_allocator)
   {
      return eastl::unique_ptr<BaseSchema>(new PoolSchemaType(p_desc, p_allocator));
   }

   ~PoolSchema()
   {
      // TODO: delete all the pages
   }

 private:
   PoolSchema(const BaseSchema::Descriptor& p_desc, BaseAllocator* p_allocator) : BaseSchema(p_desc, p_allocator)
   {
   }

   void* AllocateInternal(uint32_t p_size) final
   {
      LockScopeGuard<SpinLock> spinLock;

      const auto allocateFromPages = [m_pages]() -> void* {
         // Find a page with a free element
         Page* currentPage = m_pages;
         while (currentPage)
         {
            // Check if the page still has elements available
            if (currentPage->IsFull())
            {
               // Check if there is a free element to re-use, use that
               void* objectAddress = GetReusedElement();
               if (objectAddress == nullptr)
               {
                  // Allocate a new element
                  objectAddress = AllocateElement();

                  // Finally return the address if it's valid
                  if (objectAddress != nullptr)
                  {
                     return objectAddress;
                  }
               }
            }

            // Keep iterating
            currentPage = currentPage->m_next;
         }
      };

      // Allocate from the pages
      void* allocatedMemory = allocateFromPages();

      // Failed to allocate
      if (!allocatedMemory)
      {
         // No free elements available, allocate a new page
         AddPage(sizeof(Page));
         // Try again when a page is added
         allocatedMemory = allocateFromPages();
      }

      ASSERT(allocatedMemory != nullptr, "Failed to allocate memory");
      return allocatedMemory;
   }

   void* PoolSchema::AllocateAlignedInternal(uint32_t p_size, uint32_t p_alignment, uint32_t p_offset)
   {
      LockScopeGuard<SpinLock> spinLock;

      ASSERT(false, "Not possible to allocate aligned with the PoolSchema");
      return nullptr;
   }

   void DeallocateInternal(void* p_address, uint32_t p_size) final
   {
      LockScopeGuard<SpinLock> spinLock;

      Page* currentPage = m_pages;
      Page* previousPage = nullptr;
      while (currentPage)
      {
         const uint64_t address = static_cast<uint64_t>(p_address);

         // Get the starting and ending address
         auto [start, end] = currentPage->GetAllocatedMemoryRange();

         // Check if the address is in range
         if (address >= start && address <= end)
         {
            const uint64_t range = end - start;
            ASSERT(range % address == 0u, "The address isn't aligned correctly");

            // Free the element
            const uint32_t elementIndex = range / address;
            bool pageIsEmtpy = currentPage->FreeElement();

            // Remove the page if it's empty
            if (pageIsEmtpy && previousPage)
            {
               // Link the previous page with the upcoming page
               previousPage->m_next = currentPage->m_next;

               // Remove the page's memory
               RemovePage({.m_pageAddress = static_cast<void*>(currentPage), .m_pageSize = sizeof(Page)});
            }

            return;
         }

         // Keep iterating
         previousPage = currentPage;
         currentPage = currentPage->m_next;
      }

      ASSERT(false, "Failed to de-allocate the provided address");
   }

   PageDescriptor AddPageInternal(uint32_t p_size) final
   {
      Page* page = new Page();

      Page* emptyPage = m_pages;
      while (emptyPage->m_next)
      {
         emptyPage = emptyPage->m_next;
      }

      emptyPage->m_next = page;

      // Increment the total allocated pages
      m_pageCount++;

      return PageDescriptor{.m_pageAddress = (void*).m_pageAddress, .m_pageSize = p_size};
   }

   void RemovePageInternal(PageDescriptor& m_pageDescriptor) final
   {
      ASSERT(m_pageCount != 0u, "Trying to remove an empty page");
      m_pageCount--;

      // TODO: find a safer way
      Page* page = reinterpret_cast<Page*>(m_pageDescriptor.m_pageAddress);
      delete page;
   }

 private:
   Page* m_pages = nullptr;

   uint32_t m_pageCount = 0u;
};
}; // namespace Memory
}; // namespace Foundation
//-----------------------------------------------------------------------------
