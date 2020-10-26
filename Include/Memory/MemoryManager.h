#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <mutex>

#include <EASTL/unordered_map.h>

#include <Memory/MemoryManagerInterface.h>
#include <Std/unordered_map_bootstrap.h>

namespace Foundation
{
namespace Memory
{
// Registers all the allocators used
class MemoryManager : public MemoryManagerInterface
{
   void RegisterAllocator(Util::HashName p_hashName, AllocatorTracker* p_allocator) final;
   void UnregisterAllocator(Util::HashName p_hashName) final;

   Std::unordered_map_bootstrap<uint64_t, AllocatorTracker*> m_allocators;
   std::mutex m_registrationMutex;
};

}; // namespace Memory
}; // namespace Foundation
