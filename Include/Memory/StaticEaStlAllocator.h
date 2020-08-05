#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <type_traits>

#include <EASTL/unique_ptr.h>

namespace Foundation
{
namespace Memory
{
class BaseAllocator;

// Static EASTL allocator
class StaticEaStlAllocator
{
 public:
   StaticEaStlAllocator(const char* p_name = "StaticEaStlAllocator");
   StaticEaStlAllocator(const StaticEaStlAllocator& p_other);
   StaticEaStlAllocator(const StaticEaStlAllocator& p_other, const char* p_name);

   static void* allocate(size_t p_size, int32_t p_flag = 0);
   static void* allocate(size_t p_size, size_t p_alignment, size_t p_offset, int p_flags = 0);
   static void deallocate(void* p_address, size_t p_size);

 private:
   static void initializeAllocator();

   static eastl::unique_ptr<BaseAllocator> ms_allocator;
   static bool ms_initializedFlag;
};
}; // namespace Memory
}; // namespace Foundation
