#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <GlobalEnvironment.h>

namespace Foundation
{
namespace Memory
{
template <typename t_allocator>
class StaticAllocatorWrapper
{
 public:
   static void* Allocate(size_t p_size)
   {
      return GetAllocator().Allocate(p_size);
   }

   static void* AllocateAllign(size_t p_size, size_t p_alignment)
   {
      return GetAllocator().AllocateAllign(p_size, p_alignment);
   }

   static void Deallocate(void* p_address, size_t p_size)
   {
      GetAllocator().Deallocate(p_address, p_size);
   }

 private:
   static t_allocator& GetAllocator()
   {
      GlobalVariableRef<t_allocator> allocator = Foundation::GlobalEnvironment::CreateOrGetGlobalVariableFromType<t_allocator>();
      return *allocator.GetVariable();
   }
};
}; // namespace Memory
}; // namespace Foundation
