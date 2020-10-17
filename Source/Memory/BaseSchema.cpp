#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <memory.h>

#include <Memory/BaseAllocator.h>
#include <Memory/BaseSchema.h>

#include <Util/Assert.h>

namespace Foundation
{
namespace Memory
{
BaseSchema::BaseSchema(const SchemaDescriptor& desc, BaseAllocator* p_allocator)
{
   m_descriptor = desc;
   m_allocator = p_allocator;
}

AllocationDescriptor BaseSchema::Allocate(uint32_t p_size)
{
   return AllocateInternal(p_size);
}

AllocationDescriptor BaseSchema::AllocateAligned(uint32_t p_size, uint32_t p_alignment, uint32_t p_offset)
{
   return AllocateAlignedInternal(p_size, p_alignment, p_offset);
}

void BaseSchema::Deallocate(void* p_address, uint32_t p_size)
{
   DeallocateInternal(p_address, p_size);
}

const SchemaDescriptor& BaseSchema::GetSchemaDescriptor() const
{
   return m_descriptor;
}

}; // namespace Memory
}; // namespace Foundation
