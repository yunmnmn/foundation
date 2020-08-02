#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <memory.h>

#include <Memory/BaseSchema.h>
#include <Memory/PoolSchema.h>

#include <Util/Assert.h>

#include <EASTL/unique_ptr.h>

namespace Foundation
{
namespace Memory
{
void* PoolSchema::AllocateAlignedInternal(uint32_t p_size, uint32_t p_alignment, uint32_t p_offset)
{
   ASSERT(false, "Not possible to allocate aligned with the PoolSchema");
   return nullptr;
}

}; // namespace Memory
}; // namespace Foundation
