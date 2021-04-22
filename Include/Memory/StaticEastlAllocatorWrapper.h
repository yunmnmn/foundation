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
 public:
   StaticEastlAllocatorWrapper(const char* p_name)
   {
      UNUSED(p_name);
   }

   StaticEastlAllocatorWrapper(const StaticEastlAllocatorWrapper& p_other)
   {
      UNUSED(p_other);
   }

   StaticEastlAllocatorWrapper(const StaticEastlAllocatorWrapper& p_other, const char* p_name)
   {
      UNUSED(p_other);
      UNUSED(p_name);
   }

   bool operator!=([[maybe_unused]] const StaticEastlAllocatorWrapper& other)
   {
      // Only one bootstrap allocator, so always return true
      return true;
   }

   static void* allocate(size_t p_size, int32_t p_flag = 0)
   {
      UNUSED(p_flag);
      return GetAllocator().Allocate(static_cast<uint64_t>(p_size));
   }

   static void* allocate(size_t p_size, size_t p_alignment, size_t p_offset, int p_flags = 0)
   {
      UNUSED(p_offset);
      UNUSED(p_flags);
      return GetAllocator().AllocateAllign(static_cast<uint64_t>(p_size), static_cast<uint32_t>(p_alignment));
   }

   static void deallocate(void* p_address, size_t p_size)
   {
      GetAllocator().Deallocate(p_address, static_cast<uint64_t>(p_size));
   }

 private:
   static t_allocator& GetAllocator()
   {
      static t_allocator allocator;
      return allocator;
   }
};
}; // namespace Memory
}; // namespace Foundation
