#include <Memory/BaseAllocator.h>

namespace Foundation
{
namespace Memory
{

AllocatorTracker::AllocatorTracker(HashName p_allocatorName)
{
   // Register the allocator in the manager
   MemoryManagerInterface::Get()->RegisterAllocator(p_allocatorName, this);
}

void AllocatorTracker::TrackAllocation(AllocationDescriptor& p_desc, uint64_t p_size)
{
   std::lock_guard<std::mutex> guard(m_memoryTrackingMutex);

   // Add the descriptor if a page address is provided
   if (p_desc.m_pageAddress)
   {
      m_pages.emplace_back((uint8_t*)p_desc.m_pageAddress, p_desc.m_pageSize);
   }

   // TODO: check if this is correct
   // Find the page where the allocation is placed
   auto pageIt = eastl::find_if(m_pages.begin(), m_pages.end(), [p_desc](const Page& page) {
      if (p_desc.m_address >= page.m_pageAddress)
      {
         if (p_desc.m_address < page.m_pageAddress + page.m_pageSize)
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
   auto allocation = pageIt->m_allocations.find(p_desc.m_address);
   ASSERT(allocation == pageIt->m_allocations.end(), "Address is already used for an allocation");

   // Create the allocation, and push the allocation in the page
   Allocation alloc = {.m_allocationAddress = (uint8_t*)p_desc.m_address, .m_allocationSize = p_size};
   pageIt->m_allocations[p_desc.m_address] = alloc;
}

void AllocatorTracker::UntrackAllocation(void* p_address)
{
   std::lock_guard<std::mutex> guard(m_memoryTrackingMutex);

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

void AllocatorTracker::RemovePage(void* p_pageAddress)
{
   // TODO: finish this
   auto pageIt = eastl::find_if(m_pages.begin(), m_pages.end(),
                                [p_pageAddress](const Page& page) { return page.m_pageAddress == p_pageAddress; });
   ASSERT(pageIt != m_pages.end(), "Page isn't allocated");

   m_pages.erase(pageIt);
   // TODO do swap and pop
   // eastl::swap(pageIt, ms_pages.back());
   // ms_pages.pop_back();
}

} // namespace Memory
} // namespace Foundation
