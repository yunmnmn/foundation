#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <memory.h>

#include <Memory/BaseSchema.h>

#include <Util/Assert.h>

namespace Foundation
{
namespace Memory
{

BaseSchema::BaseSchema(const Descriptor& desc)
{
   m_descriptor = desc;
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

   // Manage the pages here

   // Call the Allocator callback
   if (m_descriptor.m_addPoolCallback)
      m_descriptor.m_addPoolCallback(pageDescriptor.m_pageAddress);
}

void BaseSchema::RemovePage(PageDescriptor& p_pageDescriptor)
{
   // Call the specialized function
   RemovePageInternal(p_pageDescriptor);

   // Call the Allocator callback
   if (m_descriptor.m_removePoolCallback)
      m_descriptor.m_removePoolCallback(p_pageDescriptor.m_pageAddress);
}

}; // namespace Memory
}; // namespace Foundation
