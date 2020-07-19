#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <EASTL/vector.h>

#include <Memory/BootstrapAllocator.h>
#include <Memory/MemoryManagerInterface.h>

#include <Util/HashName.h>

namespace Foundation
{
namespace Memory
{
// Registers all the allocators used
class MemoryManager : public MemoryManagerInterface
{
   void RegisterAllocator(AllocatorBase* allocator) final
   {
      m_allocators.emplace_back(allocator);
   }

   AllocatorBase* GetAllocatorByName()
   {
   }

   eastl::vector<AllocatorBase*, BootstrapAllocator> m_allocators;
};

}; // namespace Memory
}; // namespace Foundation
