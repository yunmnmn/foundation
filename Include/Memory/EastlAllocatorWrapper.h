#pragma once

#include <inttypes.h>
#include <stdbool.h>

namespace Foundation
{
namespace Memory
{
template <typename t_allocator>
class EastlAllocatorWrapper
{
 public:
   EastlAllocatorWrapper(const char* p_name)
   {
   }

   EastlAllocatorWrapper(const EastlAllocatorWrapper& p_other)
   {
   }

   EastlAllocatorWrapper(const EastlAllocatorWrapper& p_other, const char* p_name)
   {
   }

   bool operator!=(const BootstrapAllocator& other)
   {
      // Only one bootstrap allocator, so always return true
      return true;
   }

   void* allocate(size_t p_size, int32_t p_flag = 0)
   {
      m_allocator.Allocate(p_size);
   }

   void* allocate(size_t p_size, size_t p_alignment, size_t p_offset, int p_flags = 0)
   {
      m_allocator.AllocateAllign(p_size, p_alignment);
   }

   void deallocate(void* p_address, size_t p_size)
   {
      Deallocate(p_address, p_size);
   }

 private:
   t_allocator m_allocator;
};
}; // namespace Memory
}; // namespace Foundation
