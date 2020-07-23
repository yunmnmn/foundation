#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <Memory/BaseAllocator.h>
#include <Memory/MemoryManagerInterface.h>

#include <Util/Assert.h>

namespace Foundation
{
namespace Memory
{

void* AllocatorInterface::Allocate(uint32_t p_size)
{
   void* address = AllocateInternal(p_size);
   TrackAllocation(address, p_size);
   return address;
}

void* AllocatorInterface::AllocateAllign(uint32_t p_size, uint32_t p_alignment)
{
   void* address = AllocateAlignInternal(p_size, p_alignment);
   TrackAllocation(address, p_size);
   return address;
}

void AllocatorInterface::Deallocate(void* p_address)
{
   DeallocateInternal(p_address);
   UntrackAllocation(p_address);
}

}; // namespace Memory
}; // namespace Foundation
