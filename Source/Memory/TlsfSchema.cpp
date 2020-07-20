#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <memory.h>

#include <Memory/TlsfSchema.h>

#include <Memory/SchemaBase.h>

#include <Util/Assert.h>

#include <EASTL/unique_ptr.h>

#include <tlsf.h>

namespace Foundation
{
namespace Memory
{
static eastl::unique_ptr<TlsfSchema> TlsfSchema::CreateSchema(const SchemaBase::Descriptor& p_desc)
{
   return eastl::make_unique<TlsfSchema>(p_desc);
}

static eastl::unique_ptr<TlsfSchema> TlsfSchema::CreateSchema(const SchemaBase::Descriptor& p_desc, void* p_address)
{
   TlsfSchema* schema = new (p_address) TlsfSchema(p_desc);
   eastl::unique_ptr<TlsfSchema> test(schema);
   return eastl::move(test);
}

TlsfSchema::TlsfSchema(const TlsfSchemaDescriptor& desc)
{
   m_descriptor = desc;

   void* memory = malloc(m_descriptor.m_pageSize + sizeof(control_t));
   m_tlsf = tlsf_create_with_pool(memory);

   uint8_t* poolAddress = (uint8_t*)m_tlsf + sizeof(control_t);
   m_linkedPool = {(pool_t)poolAddress, nullptr};
}

void* TlsfSchema::Allocate(uint32_t p_size)
{
   void* address = tlsf_malloc(m_tlsf, p_size);
   if (!address)
   {
      AddPool(p_size);
      void* addedAddress = tlsf_malloc(m_tlsf, p_size);
      ASSERT(addedAddress, "Failed to alloate memory with the TLSF schema");
   }

   return address;
}

void* TlsfSchema::AllocateAligned(uint32_t p_size, uint32_t p_alignment, uint32_t p_offset)
{
   void* address = tlsf_memalign(m_tlsf, p_alignment, p_size);
   if (!address)
   {
      AddPool(p_size);
      void* addedAddress = tlsf_memalign(m_tlsf, p_alignment, p_size);
      ASSERT(addedAddress, "Failed to alloate memory with the TLSF schema");
   }
}

void TlsfSchema::Deallocate(void* p_address, uint32_t p_size)
{
   tlsf_free(m_tlsf, p_address);
}

void TlsfSchema::AddPool(uint32_t p_size)
{
   // Calculate how many pages need to be allocated
   const uint32_t pageCount = p_size / m_descriptor.m_pageSize + (p_size % m_descriptor == 0 ? 0u : 1u);
   if (m_poolCount + pageCount > m_descriptor.m_maxPageCount)
   {
      ASSERT(false, "New poolcount will exceed the limit.");
      return nullptr;
   }

   // Calculate the pool size in bytes
   const uint32_t poolSize = pageCount * m_descriptor.m_pageSize;

   // Allocate enough memory for the TlsfPool and a LinkedPool
   uint8_t* poolAddress = (uint8_t*)malloc(poolSize + sizeof(LinkedPool));

   // Add the new pool for tlsf to allocate
   tlsf_add_pool(m_tlsf, poolAddress + sizeof(LinkedPool), poolSize);

   // Find the latest entry in the linked pool
   LinkedPool* linkedPool = &m_linkedPool;
   while (linkedPool->m_next)
   {
      linkedPool = linkedPool->m_next;
   }

   // Set the new pool in the linked list
   linkedPool->m_next = poolAddress;
}

}; // namespace Memory
}; // namespace Foundation
