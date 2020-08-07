#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <memory.h>

#include <tlsf.h>

#include <Memory/BaseSchema.h>
#include <Memory/TlsfSchema.h>

#include <Util/Assert.h>

#include <Parallel/SpinLock.h>
#include <Parallel/LockScopeGuard.h>

// TODO: make it thread-safe

namespace Foundation
{
namespace Memory
{
eastl::unique_ptr<BaseSchema> TlsfSchema::CreateSchema(const BaseSchema::Descriptor& p_desc, BaseAllocator* p_allocator)
{
   return eastl::unique_ptr<BaseSchema>(new TlsfSchema(p_desc, p_allocator));
}

TlsfSchema::TlsfSchema(const BaseSchema::Descriptor& p_desc, BaseAllocator* p_allocator) : BaseSchema(p_desc, p_allocator)
{
   void* memory = malloc(tlsf_size());
   m_tlsf = tlsf_create(memory);
}

void* TlsfSchema::AllocateInternal(uint32_t p_size)
{
   LockScopeGuard<SpinLock> spinLock;

   void* address = tlsf_malloc(m_tlsf, p_size);
   if (!address)
   {
      AddPage(p_size);
      void* addedAddress = tlsf_malloc(m_tlsf, p_size);
      ASSERT(addedAddress, "Failed to alloate memory with the TLSF schema");

      return addedAddress;
   }

   return address;
}

void* TlsfSchema::AllocateAlignedInternal(uint32_t p_size, uint32_t p_alignment, uint32_t p_offset)
{
   LockScopeGuard<SpinLock> spinLock;

   void* address = tlsf_memalign(m_tlsf, p_alignment, p_size);
   if (!address)
   {
      AddPage(p_size);
      void* addedAddress = tlsf_memalign(m_tlsf, p_alignment, p_size);
      ASSERT(addedAddress, "Failed to alloate memory with the TLSF schema");

      return addedAddress;
   }

   return address;
}

void TlsfSchema::DeallocateInternal(void* p_address, uint32_t p_size)
{
   LockScopeGuard<SpinLock> spinLock;

   tlsf_free(m_tlsf, p_address);
}

BaseSchema::PageDescriptor TlsfSchema::AddPageInternal(uint32_t p_size)
{
   // Calculate how many pages need to be allocated
   // const uint32_t pageCount = p_size / m_descriptor.m_pageSize + (p_size % m_descriptor.m_pageSize == 0 ? 0u : 1u);
   const uint32_t pageCount = p_size / m_descriptor.m_pageSize + (p_size % m_descriptor.m_pageSize == 0 ? 0u : 1u);
   ASSERT(m_pageCount + pageCount <= m_descriptor.m_maxPageCount, "New poolcount will exceed the limit.");

   // Calculate the pool size in bytes
   const uint32_t pageSize = pageCount * m_descriptor.m_pageSize;

   // Allocate enough memory for the TlsfPool and a LinkedPool
   uint8_t* pageAddress = (uint8_t*)malloc(pageSize);

   // Add the new pool for tlsf to allocate
   tlsf_add_pool(m_tlsf, pageAddress, pageSize);

   // Create a new page
   PageDescriptor pageDescriptor = {pageAddress, pageSize};
   m_pageDescriptors[m_pageDescriptorIndex++] = pageDescriptor;

   return pageDescriptor;
}

void TlsfSchema::RemovePageInternal(BaseSchema::PageDescriptor& p_pageDescriptor)
{
}

}; // namespace Memory
}; // namespace Foundation
