#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <memory.h>

#include <Memory/SchemaBase.h>

#include <Util/Assert.h>

#include <EASTL/unique_ptr.h>

#include <tlsf.h>

namespace Foundation
{
namespace Memory
{
class TlsfSchema : public SchemaBase
{
   struct LinkedPool
   {
      pool_t m_pool = nullptr;
      LinkedPool m_next = nullptr;
   };

 public:
   static eastl::unique_ptr<TlsfSchema> CreateSchema(const SchemaBase::Descriptor& p_desc);
   static eastl::unique_ptr<TlsfSchema> CreateSchema(const SchemaBase::Descriptor& p_desc, void* p_address);

   void* Allocate(uint32_t p_size);
   void* AllocateAligned(uint32_t p_size, uint32_t p_alignment, uint32_t p_offset);

   void Deallocate(void* p_address, uint32_t p_size);

 private:
   TlsfSchema(const TlsfSchemaDescriptor& desc);

   // Allocate, and add the pool to the linked list
   void AddPool(uint32_t p_size);

   tlsf_t m_tlsf = nullptr;
   LinkedPool m_linkedPool = {};
   SchemaBase::Descriptor m_descriptor = {};
   uint32_t m_poolCount = 0;
};

}; // namespace Memory
}; // namespace Foundation
