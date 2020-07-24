#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <EASTL/unordered_map.h>

#include <Memory/BootstrapAllocator.h>
#include <Memory/MemoryManagerInterface.h>
#include <Memory/TlsfSchema.h>
#include <Memory/BaseAllocator.h>

namespace Foundation
{
namespace Memory
{
template <typename t_key, typename t_value>
using unordered_map = eastl::unordered_map<t_key, t_value, eastl::hash<t_key>, eastl::equal_to<t_key>,
                                           Memory::BootstrapAllocator<Memory::TlsfSchema>, false>;

// Registers all the allocators used
class MemoryManager : public MemoryManagerInterface
{
   void RegisterAllocator(HashName p_hashName, BaseAllocator* p_allocator) final;
   void UnregisterAllocator(HashName p_hashName) final;

   unordered_map<uint64_t, BaseAllocator*> m_allocators;
};

}; // namespace Memory
}; // namespace Foundation
