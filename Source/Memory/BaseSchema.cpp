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
BaseSchema::BaseSchema(const Descriptor& desc, BaseAllocator* p_allocator)
{
   m_descriptor = desc;
   m_allocator = p_allocator;
}

void* BaseSchema::Allocate(uint32_t p_size)
{
   return AllocateInternal(p_size);
}

void BaseSchema::Deallocate(void* p_address, uint32_t p_size)
{
   DeallocateInternal(p_address, p_size);
}

void* BaseSchema::AllocateAligned(uint32_t p_size, uint32_t p_alignment, uint32_t p_offset)
{
   return AllocateAlignedInternal(p_size, p_alignment, p_offset);
}

void BaseSchema::AddPage(uint32_t p_size)
{
   // Call the specialized function
   PageDescriptor pageDescriptor = AddPageInternal(p_size);

   // Call the BaseAllocator's AddPage function
   if (m_allocator)
      m_allocator->AddPage(pageDescriptor);
}

void BaseSchema::RemovePage(PageDescriptor& p_pageDescriptor)
{
   // Call the specialized function
   RemovePageInternal(p_pageDescriptor);

   // Call the BaseAllocator's RemovePage function
   if (m_allocator)
      m_allocator->RemovePage(p_pageDescriptor.m_pageAddress);
}

}; // namespace Memory
}; // namespace Foundation
