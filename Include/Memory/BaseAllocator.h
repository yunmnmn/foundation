#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <functional>

#include <Memory/BootstrapAllocator.h>
#include <Memory/BaseSchema.h>
#include <Memory/TlsfSchema.h>

#include <Util/HashName.h>

#include <EASTL/unique_ptr.h>
#include <EASTL/unordered_map.h>
#include <EASTL/vector.h>
#include <EASTL/algorithm.h>

namespace Foundation
{
namespace Memory
{

class BaseAllocator
{
   template <typename t_key, typename t_value>
   using unordered_map =
       eastl::unordered_map<t_key, t_value, eastl::hash<t_key>, eastl::equal_to<t_key>, BootstrapAllocator<TlsfSchema>, false>;

   template <typename t_value> using vector = eastl::vector<t_value, BootstrapAllocator<TlsfSchema>>;

 public:
   struct Allocation
   {
      uint8_t* m_allocationAddress = nullptr;
      uint64_t m_allocationSize = 0u;
   };

   struct Page
   {
      uint8_t* m_pageAddress = nullptr;
      uint64_t m_pageSize = 0u;

      unordered_map<void*, Allocation> m_allocations;
   };

   void* Allocate(uint64_t p_size);
   void* AllocateAllign(uint64_t p_size, uint32_t p_alignment);
   void Deallocate(void* p_address, uint64_t p_size);

   void AddPage(BaseSchema::PageDescriptor p_pageDescriptor);
   void RemovePage(void* p_pageAddress);

 protected:
   BaseAllocator(HashName p_allocatorName, eastl::unique_ptr<BaseSchema> p_schema);

   void TrackAllocation(void* p_address, uint64_t p_size);
   void UntrackAllocation(void* p_address);

   uint32_t GetPageCount() const;

   virtual void* AllocateInternal(uint64_t p_size) = 0;
   virtual void* AllocateAlignInternal(uint64_t p_size, uint32_t p_alignment) = 0;
   virtual void DeallocateInternal(void* p_pointer, uint64_t p_size) = 0;

   vector<Page> m_pages;
   eastl::unique_ptr<BaseSchema> m_schema = nullptr;
   HashName m_name;
};

}; // namespace Memory
}; // namespace Foundation
