#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <functional>

#include <Memory/BootstrapAllocator.h>
#include <Memory/BaseSchema.h>
#include <Memory/MemoryManagerInterface.h>
#include <Memory/TlsfSchema.h>

#include <Util/HashName.h>

#include <EASTL/unordered_map.h>
#include <EASTL/vector.h>
#include <EASTL/algorithm.h>

#include <Util/Assert.h>

namespace Foundation
{
namespace Memory
{
// A single tracked allocation within a page
struct Allocation
{
   uint8_t* m_allocationAddress = nullptr;
   uint64_t m_allocationSize = 0u;
};

// A single tracked page
struct Page
{
   Page(uint8_t* pageAddress, uint64_t pageSize)
   {
      m_pageAddress = pageAddress;
      m_pageSize = pageSize;
   }

   uint8_t* m_pageAddress = nullptr;
   uint64_t m_pageSize = 0u;

   unordered_map<void*, Allocation> m_allocations;
};

template <typename t_key, typename t_value>
using unordered_map =
    eastl::unordered_map<t_key, t_value, eastl::hash<t_key>, eastl::equal_to<t_key>, EastlBootstrapAllocator, false>;

template <typename t_value>
using vector = eastl::vector<t_value, EastlBootstrapAllocator>;

using PageIt = vector<Page>::iterator;

// Tracks allocation, and registers itself
class AllocatorTracker
{
 public:
   uint32_t GetPageCount() const
   {
      return static_cast<uint32_t>(m_pages.size());
   }

 protected:
   AllocatorTracker() = delete;
   // TODO change this
   AllocatorTracker(HashName p_allocatorName);

   // Track allocated allocations from the schema
   void TrackAllocation(AllocationDescriptor& p_address, uint64_t p_size);

   // Untrack allocated allocations from the schema
   void UntrackAllocation(void* p_address);

   // Finds the page, and returns the pointer and index in the list
   PageIt FindPageFromAddress(void* p_address);

   // Manges an added allocation
   void AddPageForTracking(PageDescriptor&& p_pageDescriptor);

   // Remove the page from the index
   void RemovePageFromTracking(PageIt pageIt);

   vector<Page> m_pages;

   std::mutex m_memoryTrackingMutex;
};

//
template <const char* t_name, typename t_schema>
class BaseAllocator : public AllocatorTracker
{
 public:
   BaseAllocator() : AllocatorTracker(HashName(t_name))
   {
   }

   void* Allocate(uint64_t p_size)
   {
      AllocationDescriptor desc = AllocateInternal(p_size);
      TrackAllocation(desc, p_size);
      return desc.m_address;
   }

   void* AllocateAllign(uint64_t p_size, uint32_t p_alignment)
   {
      AllocationDescriptor desc = AllocateAlignInternal(p_size, p_alignment);
      TrackAllocation(desc, p_size);
      return desc.m_address;
   }

   void Deallocate(void* p_address, uint64_t p_size)
   {
      DeallocateInternal(p_address, p_size);
      UntrackAllocation(p_address);
   }

   constexpr const char* const GetAllocatorName() const
   {
      return t_name;
   }

 protected:
   t_schema m_schema;

 private:
   // Allocator specific allocation
   virtual AllocationDescriptor AllocateInternal(uint64_t p_size) = 0;
   // Allocator specific aligned allocation
   virtual AllocationDescriptor AllocateAlignInternal(uint64_t p_size, uint32_t p_alignment) = 0;
   // Allocator specific deallocation
   virtual void DeallocateInternal(void* p_pointer, uint64_t p_size) = 0;
};

template <const char* t_name, typename t_schema>
class DefaultAllocator : public BaseAllocator<t_name, t_schema>
{
   using BaseAllocatorType = BaseAllocator<t_name, t_schema>;

 public:
   DefaultAllocator() = default;

   AllocationDescriptor AllocateInternal(uint64_t p_size) final
   {
      return m_schema.Allocate(p_size);
   }

   AllocationDescriptor AllocateAlignInternal(uint64_t p_size, uint32_t p_alignment) final
   {
      // TODO: 0u, huh?
      return m_schema.AllocateAligned(p_size, p_alignment, 0u);
   }

   void DeallocateInternal(void* p_address, uint64_t p_size) final
   {
      m_schema.Deallocate(p_address, p_size);
   }
};

}; // namespace Memory
}; // namespace Foundation
