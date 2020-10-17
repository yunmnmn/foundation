#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <type_traits>

#include <EASTL/unique_ptr.h>

#include <Util/Assert.h>
#include <Util/Util.h>

#include <Memory/BaseSchema.h>

namespace Foundation
{
namespace Memory
{
// Unique allocator, memory allocated on this isn't tracked
template <typename t_schema>
class BootstrapAllocator
{
   using AllocationDataType = std::aligned_storage<sizeof(t_schema), std::alignment_of<t_schema>::value>::type;

 public:
   BootstrapAllocator(const char* pName)
   {
   }

   BootstrapAllocator(const BootstrapAllocator& x)
   {
   }

   BootstrapAllocator(const BootstrapAllocator& x, const char* pName)
   {
   }

   bool operator!=(const BootstrapAllocator& other)
   {
      // Only one bootstrap allocator, so always return true
      return true;
   }

   static void* allocate(size_t p_size, int32_t p_flag = 0)
   {
      AllocationDescriptor desc = ms_schema.Allocate(p_size);
      return desc.m_address;
   }

   static void* allocate(size_t p_size, size_t p_alignment, size_t offset, int flags = 0)
   {
      AllocationDescriptor desc = ms_schema.AllocateAligned(p_size, p_alignment, offset);
      return desc.m_address;
   }

   static void deallocate(void* p, size_t n)
   {
      ms_schema.Deallocate(p, n);
   }

 private:
   static t_schema ms_schema;
};

template <typename t_schema>
t_schema BootstrapAllocator<t_schema>::ms_schema;

}; // namespace Memory
}; // namespace Foundation
