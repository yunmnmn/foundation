#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <Memory/BaseAllocator.h>
#include <Memory/MemoryManagerInterface.h>

#include <Util/Assert.h>

namespace Foundation
{
namespace Memory
{
BaseAllocator::BaseAllocator(HashName p_allocatorName, eastl::unique_ptr<BaseSchema> p_schema)
{
   // Register the allocator in the manager
   MemoryManagerInterface::Get()->RegisterAllocator(p_allocatorName, this);

   // Set the schema
   ASSERT(p_schema != nullptr, "Schema must be valid");
   m_schema = eastl::move(p_schema);

   // Set the name
   m_name = p_allocatorName;
}

void* BaseAllocator::Allocate(uint64_t p_size)
{
   AllocationDescriptor desc = AllocateInternal(p_size);
   TrackAllocation(desc, p_size);
   return desc.m_address;
}

void* BaseAllocator::AllocateAllign(uint64_t p_size, uint32_t p_alignment)
{
   AllocationDescriptor desc = AllocateAlignInternal(p_size, p_alignment);
   TrackAllocation(desc, p_size);
   return desc.m_address;
}

void BaseAllocator::Deallocate(void* p_address, uint64_t p_size)
{
   DeallocateInternal(p_address, p_size);
   UntrackAllocation(p_address);
}

uint32_t BaseAllocator::GetPageCount() const
{
   return 0u;
}

void BaseAllocator::TrackAllocation(AllocationDescriptor& p_address, uint64_t p_size)
{
   // Check if it's a new page

   // Find the page where the allocation is placed
   auto pageIt = eastl::find_if(m_pages.begin(), m_pages.end(), [p_address](const Page& page) {
      if (p_address.>= page.m_pageAddress)
      {
         if (p_address < page.m_pageAddress + page.m_pageSize)
         {
            return true;
         }
         return false;
      }
      else
      {
         return false;
      }
   });
   ASSERT(pageIt != m_pages.end(), "Allocation doesn't fit in the page");

   // Validate that the address hasn't been allocated in the map
   auto allocation = pageIt->m_allocations.find(p_address);
   ASSERT(allocation == pageIt->m_allocations.end(), "Address is already used for an allocation");

   // Create the allocation, and push the allocation in the page
   Allocation alloc = {.m_allocationAddress = (uint8_t*)p_address, .m_allocationSize = p_size};
   pageIt->m_allocations[p_address] = alloc;
}

void BaseAllocator::UntrackAllocation(void* p_address)
{
   // Find the page where the allocation is placed
   auto pageIt = eastl::find_if(m_pages.begin(), m_pages.end(), [p_address](const Page& page) {
      if (p_address >= page.m_pageAddress)
      {
         if (p_address < page.m_pageAddress + page.m_pageSize)
         {
            return true;
         }
         return false;
      }
      else
      {
         return false;
      }
   });
   ASSERT(pageIt != m_pages.end(), "Allocation doesn't fit in the page");

   // Validate that the address hasn't been allocated in the map
   auto allocation = pageIt->m_allocations.find(p_address);
   ASSERT(allocation != pageIt->m_allocations.end(), "Address isn't used to allocate yet");

   // Delete the entry from the map
   pageIt->m_allocations.erase(allocation);
}

void BaseAllocator::AddPage(PageDescriptor p_pageDescriptor)
{
   // Make sure that the page isn't allocated already
   Page page;
   page.m_pageAddress = (uint8_t*)p_pageDescriptor.m_pageAddress;
   page.m_pageSize = p_pageDescriptor.m_pageSize;
   m_pages.emplace_back(page);
}

void BaseAllocator::RemovePage(void* p_pageAddress)
{
   auto pageIt = eastl::find_if(m_pages.begin(), m_pages.end(),
                                [p_pageAddress](const Page& page) { return page.m_pageAddress == p_pageAddress; });
   ASSERT(pageIt != m_pages.end(), "Page isn't allocated");

   m_pages.erase(pageIt);
   // TODO do swap and pop
   // eastl::swap(pageIt, ms_pages.back());
   // ms_pages.pop_back();
}

}; // namespace Memory
}; // namespace Foundation
