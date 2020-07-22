#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <Memory/BaseAllocator.h>

#include <Util/Assert.h>

namespace Foundation
{
namespace Memory
{

uint32_t BaseAllocator::GetPageCount() const
{
   return 0u;
}

void* BaseAllocator::Allocate(uint32_t p_size)
{
   return nullptr;
}

void* BaseAllocator::AllocateAllign(uint32_t p_size, uint32_t p_alignment)
{
   return nullptr;
}

void BaseAllocator::Deallocate(void* p_address)
{
}

}; // namespace Memory
}; // namespace Foundation
