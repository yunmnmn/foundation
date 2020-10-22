#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <atomic>
#include <mutex>

#include <Util/Assert.h>
#include <Util/Macro.h>

#include <Memory/BaseSchema.h>

#include <EASTL/unique_ptr.h>
#include <EASTL/tuple.h>

namespace Foundation
{
namespace Memory
{
template <typename t_elementType, size_t t_pageCount, size_t t_pageElementCount>
class PoolSchema : public BaseSchema<t_pageCount, sizeof(t_elementType) * t_pageElementCount>
{
   class Page
   {
      static constexpr uint32_t InvalidElementIndex = static_cast<uint32_t>(-1);

    public:
      Page()
      {
         const uint64_t size = sizeof(t_elementType) * t_pageElementCount;
         m_objects = static_cast<t_elementType*>(::malloc(size));
      }

      ~Page()
      {
         ::free(m_objects);
      }

      void* GetMemoryAddress(uint32_t p_index)
      {
         t_elementType* freeObject = &m_objects[p_index];
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

         return objectAddress;
      }

      // Allocate an element that hasn't been used yet
      inline void* AllocateElement()
      {
         if (m_elementIndex < t_pageElementCount)
         {
            // increment the element index
            const uint32_t index = m_elementIndex++;

            void* objectAddress = GetMemoryAddress(index);

            // Increment the total amount of allocated elements
            m_elementCount++;

            return objectAddress;
         }

         return nullptr;
      }

      // Frees an element in the page. Returns true if the page is empty (no more allocations)
      inline bool FreeElement(uint32_t p_index)
      {
         // TODO
         // ASSERT(m_nextIndex[p_index] == InvalidElementIndex, "This index should be invalid");

         m_nextIndex[p_index] = m_freeElementIndex;
         m_freeElementIndex = p_index;
         m_elementCount--;

         if (m_elementCount == 0)
         {
            return true;
         }

         return false;
      }

      // Checks if the page is already full
      inline bool IsFull()
      {
         return m_elementCount >= t_pageElementCount;
      }

      inline bool IsEmpty()
      {
         return m_elementCount == 0u;
      }

      // TODO: make this a constexpr
      auto GetAllocatedMemoryRange()
      {
         // TODO: dangerous
         const uint64_t start = reinterpret_cast<uint64_t>(&m_objects[0u]);
         const uint64_t end = reinterpret_cast<uint64_t>(&m_objects[t_pageElementCount - 1u]);
         ASSERT(end > start, "The allocated page range isn't correct");
         return eastl::make_tuple(start, end);
      }

      uint32_t GetElementCount() const
      {
         return m_elementCount;
      }

      // Pointer to next page
      Page* m_next = nullptr;

    private:
      // Array of objects
      t_elementType* m_objects;
      // List of elements that link to the next free index
      uint32_t m_nextIndex[t_pageElementCount] = {InvalidElementIndex};

      // Current element index
      uint32_t m_elementIndex = 0u;
      // Total allocated elements in the page
      uint32_t m_elementCount = 0u;
      // The first free index
      uint32_t m_freeElementIndex = InvalidElementIndex;
   };

   using PoolSchemaType = PoolSchema<t_elementType, t_pageCount, t_pageElementCount>;

 public:
   PoolSchema() = default;
   ~PoolSchema()
   {
      Page* currentPage = m_pages;
      while (currentPage)
      {
         Page* cachedPage = currentPage;
         currentPage = currentPage->m_next;

         delete cachedPage;
         m_pageCount--;
      }

      ASSERT(m_pageCount == 0u, "Not all pages are deleted");
   }

 private:
   AllocationDescriptor AllocateInternal(uint64_t p_size) final
   {
      ASSERT(p_size <= sizeof(t_elementType), "The requested size is larger than the element type");
      std::lock_guard<std::mutex> guard(m_schameMutex);

      const auto allocateFromPages = [this]() -> void* {
         // Find a page with a free element
         Page* currentPage = m_pages;
         while (currentPage)
         {
            // Check if the page still has elements available
            if (!currentPage->IsFull())
            {
               // Check if there is a free element to re-use
               void* objectAddress = m_pages->GetReusedElement();
               if (objectAddress)
               {
                  return objectAddress;
               }
               else
               {
                  // Allocate a new element
                  objectAddress = m_pages->AllocateElement();

                  // Finally return the address if it's valid
                  if (objectAddress)
                  {
                     return objectAddress;
                  }
               }
            }

            // Keep iterating
            currentPage = currentPage->m_next;
         }

         return nullptr;
      };

      void* address = allocateFromPages();

      // Allocate from the pages
      AllocationDescriptor allocationDescriptor = {.m_address = address, .m_size = p_size};

      // Failed to allocate
      if (!address)
      {
         // No free elements available, allocate a new page
         Page* page = AddPage();

         // Try again when a page is added
         allocationDescriptor = AllocationDescriptor{.m_address = page->AllocateElement(),
                                                     .m_size = p_size,
                                                     .m_pageAddress = page->GetMemoryAddress(0),
                                                     .m_pageSize = GetPageSize()};
      }

      ASSERT(allocationDescriptor.m_address != nullptr, "Failed to allocate memory");
      return allocationDescriptor;
   }

   AllocationDescriptor PoolSchema::AllocateAlignedInternal(uint64_t p_size, uint32_t p_alignment, uint64_t p_offset) final
   {
      std::lock_guard<std::mutex> guard(m_schameMutex);

      ASSERT(false, "Not possible to allocate aligned with the PoolSchema");
      return AllocationDescriptor{};
   }

   void DeallocateInternal(void* p_address, uint64_t p_size) final
   {
      std::lock_guard<std::mutex> guard(m_schameMutex);

      Page* currentPage = m_pages;
      Page* previousPage = nullptr;
      while (currentPage)
      {
         // TODO: dangerous, change to pointer specific arithmetic
         const uint64_t address = reinterpret_cast<uint64_t>(p_address);

         // Get the starting and ending address
         const auto [start, end] = currentPage->GetAllocatedMemoryRange();

         // Check if the address is in range
         if (address >= start && address <= end)
         {
            const uint64_t relative = address - start;
            const uint32_t typeSize = sizeof(t_elementType);

            ASSERT((relative % typeSize) == 0u, "addres is not aligned");

            // Free the element
            const uint32_t elementIndex = static_cast<uint32_t>(relative / typeSize);
            const bool pageIsEmtpy = currentPage->FreeElement(elementIndex);

            // Remove the page if it's empty
            if (pageIsEmtpy)
            {
               // If the chain is severed in the middle or end
               if (previousPage)
               {
                  // Link the previous page with the upcoming page
                  previousPage->m_next = currentPage->m_next;
               }
               // If the page is severed at the front
               else
               {
                  m_pages = currentPage->m_next;
               }

               // Remove the page's memory
               RemovePage(currentPage);
            }

            return;
         }

         // Keep iterating
         previousPage = currentPage;
         currentPage = currentPage->m_next;
      }

      ASSERT(false, "Failed to de-allocate the provided address");
   }

   Page* AddPage()
   {
      ASSERT(m_pageCount < t_pageCount, "Trying to allocate too many pages");

      // Allocate a new page
      Page* page = new Page();

      Page* pages = m_pages;
      if (!pages)
      {
         m_pages = page;
      }
      else
      {
         while (pages->m_next)
         {
            pages = pages->m_next;
         }

         pages->m_next = page;
      }

      // Increment the total allocated pages
      m_pageCount++;

      return page;
   }

   void RemovePage(Page* page)
   {
      ASSERT(page != nullptr, "page isn't valid");
      ASSERT(m_pageCount != 0u, "Trying to remove an empty page");
      ASSERT(page->GetElementCount() == 0u, "Trying to remove an empty page");

      // Free the memory
      delete page;

      // Decrement the page count
      m_pageCount--;
   }

   constexpr uint64_t GetPageSize() const
   {
      return sizeof(t_elementType) * t_pageElementCount;
   }

 private:
   Page* m_pages = nullptr;
   uint32_t m_pageCount = 0u;

   std::mutex m_schameMutex;
};
}; // namespace Memory
}; // namespace Foundation
//-----------------------------------------------------------------------------
