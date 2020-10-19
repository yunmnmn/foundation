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

      inline bool FreeElement(uint32_t p_index)
      {
         // TODO
         // ASSERT(m_nextIndex[p_index] == InvalidElementIndex, "This index should be invalid");

         m_nextIndex[p_index] = m_freeElementIndex;
         m_freeElementIndex = p_index;
         m_elementCount--;

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

      // Pointer to next page
      Page* m_next = nullptr;

    private:
      // Array of objects
      t_elementType m_objects[t_pageElementCount] = {};
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
      // TODO: delete all the pages
   }

 private:
   AllocationDescriptor AllocateInternal(uint64_t p_size) final
   {
      ASSERT(p_size < sizeof(t_elementType), "The requested size is larger than the element type");
      std::lock_guard<std::mutex> guard(m_schameMutex);

      const auto allocateFromPages = [this]() -> void* {
         // Find a page with a free element
         Page* currentPage = m_pages;
         while (currentPage)
         {
            // Check if the page still has elements available
            if (currentPage->IsFull())
            {
               // Check if there is a free element to re-use, use that
               void* objectAddress = m_pages->GetReusedElement();
               if (objectAddress == nullptr)
               {
                  // Allocate a new element
                  objectAddress = m_pages->AllocateElement();

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

         // TODO: not sure
         return nullptr;
      };

      // Allocate from the pages
      AllocationDescriptor allocationDescriptor = {.m_address = allocateFromPages()};

      // Failed to allocate
      if (!allocationDescriptor.m_address)
      {
         // No free elements available, allocate a new page
         PageDescriptor pageDescriptor = AddPage(sizeof(Page));
         // Try again when a page is added

         allocationDescriptor = AllocationDescriptor{
             .m_address = allocateFromPages(), .m_size = p_size, .m_pageAddress = pageDescriptor.m_pageAddress};
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
         // TODO: dangerous
         const uint64_t address = reinterpret_cast<uint64_t>(p_address);

         // Get the starting and ending address
         auto [start, end] = currentPage->GetAllocatedMemoryRange();

         // Check if the address is in range
         if (address >= start && address <= end)
         {
            const uint64_t range = end - start;
            ASSERT(range % address == 0u, "The address isn't aligned correctly");

            // Free the element
            const uint32_t elementIndex = static_cast<uint32_t>(range / address);
            bool pageIsEmtpy = currentPage->FreeElement(elementIndex);

            // Remove the page if it's empty
            if (pageIsEmtpy && previousPage)
            {
               // Link the previous page with the upcoming page
               previousPage->m_next = currentPage->m_next;

               // Remove the page's memory
               RemovePage(PageDescriptor{.m_pageAddress = static_cast<void*>(currentPage), .m_pageSize = sizeof(Page)});
            }

            return;
         }

         // Keep iterating
         previousPage = currentPage;
         currentPage = currentPage->m_next;
      }

      ASSERT(false, "Failed to de-allocate the provided address");
   }

   PageDescriptor AddPage(uint64_t p_size)
   {
      ASSERT(m_pageCount < t_pageCount, "Trying to allocate too many pages");

      Page* page = new Page();

      Page* emptyPage = m_pages;
      while (emptyPage->m_next)
      {
         emptyPage = emptyPage->m_next;
      }

      emptyPage->m_next = page;

      // Increment the total allocated pages
      m_pageCount++;

      return PageDescriptor{.m_pageAddress = (void*)page, .m_pageSize = p_size};
   }

   void RemovePage(PageDescriptor m_pageDescriptor)
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

   std::mutex m_schameMutex;
};
}; // namespace Memory
}; // namespace Foundation
//-----------------------------------------------------------------------------
