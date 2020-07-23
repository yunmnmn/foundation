#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <Memory/EaStlAllocator.h>
#include <EASTL/unordered_map.h>

namespace Foundation
{
namespace Memory
{
class BaseAllocator
{
   template <typename t_key, typename t_value>
   using unordered_map = eastl::unordered_map<t_key, t_value, eastl::hash<t_key>, eastl::equal_to<t_key>, EaStlAllocator, false>;

   struct Allocation
   {
      uint8_t* m_allocationAddress = nullptr;
      uint32_t m_allocationSize = 0u;
   };

   BaseAllocator();

   virtual void* AllocateInternal(uint32_t p_size) = 0;
   virtual void* AllocateAlignInternal(uint32_t p_size, uint32_t p_alignment) = 0;
   virtual void DeallocateInternal(void* p_pointer) = 0;

   uint32_t GetPageCount() const;

   void* Allocate(uint32_t p_size);
   void* AllocateAllign(uint32_t p_size, uint32_t p_alignment);
   void Deallocate(void* p_address);

   // Allocation Address -> Allocation Size
   unordered_map<void*, uint32_t> m_allocations;
};

}; // namespace Memory
}; // namespace Foundation
