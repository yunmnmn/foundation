#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <Util/Assert.h>
#include <Memory/MemoryManagerInterface.h>

/*
// Define a few things here:
overload new
overload delete
restrict use of array new
restrict use of array delete

static allocator
allocator manager
*/

namespace Foundation
{
namespace Memory
{
template <typename t_class, typename t_allocator> class AllocatorInterface
{
   static t_allocator* Get()
   {
      if (!ms_allocator)
      {
         t_allocator::Descriptor descriptor;
         t_class::InitializePoolDescriptor(descriptor);
         ms_allocator = new (ms_allocatorData) t_allocator(descriptor);
         MemoryManagerInterface::Get()->RegisterAllocator(ms_allocator);
      }

      return ms_allocator;
   }

   static void Allocate()
   {
      Get()->Allocate();
   }

   static void Deallocate(void* data, uint32_t size)
   {
   }

   t_allocator* ms_allocator = nullptr;
   static std::aligned_storage<sizeof(t_allocator), std::alignment_of<t_allocato>>::value > ::type ms_allocatorData = {};
};

}; // namespace Memory
}; // namespace Foundation
//-----------------------------------------------------------------------------
