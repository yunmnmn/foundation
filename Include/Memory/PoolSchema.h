#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <atomic>

#include <Util/Assert.h>
#include <Util/Macro.h>
#include <Memory/BaseSchema.h>

#include <EASTL/unique_ptr.h>

namespace Foundation
{
namespace Memory
{
class BaseAllocator;

template <typename t_elementType, size_t t_pageSize, size_t t_pageCount>
class PoolSchema : public BaseSchema
{
   struct Page
   {
      // Current element index
      uint32_t m_elementIndex = 0u;
      // Total allocated elements in the page
      uint32_t m_elementCount = 0u;
      // The first free index
      uint32_t m_freeElementIndex = InvalidElementIndex;

      t_elementType m_objects[t_pageSize] = {};
      uint32_t m_nextIndex[t_pageSize] = {InvalidElementIndex};

      // Pointer to next page
      Page* m_next = nullptr;
   };

   const uint32_t InvalidElementIndex = static_cast<uint32_t>(-1);

 public:
   static eastl::unique_ptr<BaseSchema> CreateSchema(const BaseSchema::Descriptor& p_desc, BaseAllocator* p_allocator)
   {
      return eastl::make_unique<PoolSchema<t_elementType, t_pageSize, t_pageCount>>(p_desc, p_allocator);
   }

   PoolSchema(const BaseSchema::Descriptor& p_desc, BaseAllocator* p_allocator) : BaseSchema(p_desc, p_allocator)
   {
      // TODO: delete all the pages
   }

   ~PoolSchema();

 private:
   void* AllocateInternal(uint32_t p_size) final
   {
      // Wait till the flag is free
      while (m_pagesFlag.test_and_set())
      {
      }

      const auto allocateFromPages = [m_pages]() -> void* {
         // Find a page with a free element
         Page* currentPage = m_pages;
         while (currentPage)
         {
            // Check if the page still has elements available
            if (currentPage->m_elementCount < t_pageSize)
            {
               // Check if there is a free element
               if (currentPage->m_freeElementIndex != InvalidElementIndex)
               {
                  // Get the address of the index that is free
                  t_elementType* freeObject = &currentPage->m_objects[currentPage->m_freeElementIndex];

                  // Set the next free index
                  const uint32_t previousFreeIndex = currentPage->m_freeElementIndex;
                  currentPage->m_freeElementIndex = currentPage->m_nextIndex[currentPage->m_freeElementIndex];

                  // Set the current one
                  currentPage->m_nextIndex[previousFreeIndex] = InvalidElementIndex;

                  currentPage->m_elementCount++;

                  // Re-use an object
                  return freeObject;
               }
               else
               {
                  if (m_elementIndex < t_pageSize)
                  {
                     // increment the element index
                     const uint32_t index = m_elementIndex++;

                     // TODO:"get the address related to the index
                     t_elementType* freeObject = &currentPage->m_objects[index];

                     // Increment the total amount of allocated elements
                     currentPage->m_elementCount++;
                  }
               }
            }

            // Check if the page is locked
            currentPage = currentPage->m_next;
         }

         return nullptr;
      };

      // Allocate from the pages
      void* allocatedMemory = allocateFromPages();

      // Failed to allocate
      if (!allocatedMemory)
      {
         // No free elements available, allocate a new page
         AddPageInternal(0);
         // Try again when a page is added
         allocatedMemory = allocateFromPages();
      }

      m_pagesFlag.clear();

      ASSERT(allocatedMemory != nullptr, "Failed to allocate memory");
      return allocatedMemory;
   }

   void* AllocateAlignedInternal(uint32_t p_size, uint32_t p_alignment, uint32_t p_offset) final
   {
      void* PoolSchema::AllocateAlignedInternal(uint32_t p_size, uint32_t p_alignment, uint32_t p_offset)
      {
         ASSERT(false, "Not possible to allocate aligned with the PoolSchema");
         return nullptr;
      }
   }

   void DeallocateInternal(void* p_address, uint32_t p_size) final
   {
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

      return PageDescriptor{.m_pageAddress = (void*).m_pageAddress, .m_pageSize = sizeof(Page)};
   }

   void RemovePageInternal(PageDescriptor& m_pageDescriptor) final
   {
   }

 private:
   Page* m_pages = nullptr;
   std::atomic_flag m_pagesFlag;

   uint32_t m_pageCount = 0u;
};
}; // namespace Memory
}; // namespace Foundation
//-----------------------------------------------------------------------------
