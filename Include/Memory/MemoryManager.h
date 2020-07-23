#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <EASTL/vector.h>

#include <Memory/BootstrapAllocator.h>
#include <Memory/MemoryManagerInterface.h>
#include <Memory/TlsfSchema.h>

#include <Util/HashName.h>

namespace Foundation
{
namespace Memory
{
// Registers all the allocators used
class MemoryManager : public MemoryManagerInterface
{
   void RegisterAllocator(BaseAllocator* allocator) final;
   void UnregisterAllocator() final;

   BaseAllocator* GetAllocatorByName();

   eastl::vector<BaseAllocator*, BootstrapAllocator<TlsfSchema>> m_allocators;
};

}; // namespace Memory
}; // namespace Foundation
