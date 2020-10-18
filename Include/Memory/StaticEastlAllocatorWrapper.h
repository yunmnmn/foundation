#pragma once

#include <inttypes.h>
#include <stdbool.h>

namespace Foundation
{
namespace Memory
{
template <typename t_allocator>
class StaticEastlAllocatorWrapper
{
   using AllocatorDataType = typename std::aligned_storage<sizeof(t_allocator), std::alignof(t_allocator)>::type;

 public:
   StaticEastlAllocatorWrapper(const char* p_name)
   {
   }

   StaticEastlAllocatorWrapper(const StaticEastlAllocatorWrapper& p_other)
   {
   }

   StaticEastlAllocatorWrapper(const StaticEastlAllocatorWrapper& p_other, const char* p_name)
   {
   }

   bool operator!=(const BootstrapAllocator& other)
   {
      // Only one bootstrap allocator, so always return true
      return true;
   }

   static void* allocate(size_t p_size, int32_t p_flag = 0)
   {
      initializeAllocator(p_name);
      return m_allocator->Allocate(p_size);
   }

   static void* allocate(size_t p_size, size_t p_alignment, size_t p_offset, int p_flags = 0)
   {
      initializeAllocator(p_name);
      return m_allocator->AllocateAllign(p_size, p_alignment);
   }

   static void deallocate(void* p_address, size_t p_size)
   {
      initializeAllocator(p_name);
      m_allocator->Deallocate(p_address, p_size);
   }

 private:
   static void initializeAllocator(const char* p_name)
   {
      // TODO: Replace this with a double checked
      // TODO: placement new to create an instance of the allocator
      CallOnce(ms_initializedFlag,
               []() { ms_allocator = eastl::move(StaticEastlAllocatorWrapper<t_class, t_schema>::CreateAllocator(p_name)); });
   }

   static t_allocator* ms_allocator;
   static StaticAllocatorWrapper ms_allocatorData = {};

   static bool ms_initializedFlag;
};
template <typename t_allocator>
t_allocator* StaticEastlAllocatorWrapper<t_allocator>::ms_allocator;
template <typename t_allocator>
StaticEastlAllocatorWrapper<t_allocator>::AllocatorDataType StaticEastlAllocatorWrapper<t_allocator>::ms_allocatorData;
template <typename t_allocator>
bool StaticEastlAllocatorWrapper<t_allocator>::ms_initializedFlag = false;
}; // namespace Memory
}; // namespace Foundation
