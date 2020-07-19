#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <Memory/SchemaBase.h>

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
      pool_t m_next = nullptr;
   };

 public:
   static eastl::unique_ptr<TlsfSchema> CreateSchema(const SchemaBase::Descriptor& p_desc)
   {
      return eastl::make_unique<TlsfSchema>(p_desc);
   }

   static eastl::unique_ptr<TlsfSchema> CreateSchema(const SchemaBase::Descriptor& p_desc, void* p_address)
   {
      TlsfSchema* schema = new (p_address) TlsfSchema(p_desc);
      eastl::unique_ptr<TlsfSchema> test(schema);
      return eastl::move(test);
   }

   void* Allocate(uint32_t p_size)
   {
   }

   void* AllocateAligned(uint32_t p_size, uint32_t p_alignment, uint32_t p_offset)
   {
   }

   void Deallocate(void* p_address, uint32_t p_size)
   {
   }

 private:
   TlsfSchema(const TlsfSchemaDescriptor& desc)
   {
      m_descriptor = desc;

      void* memory = malloc();
      m_tlsf = tlsf_create_with_pool(memory);
   }

   tlsf_t m_tlsf = nullptr;
   LinkedPool m_linkedPool = {};
   SchemaBase::Descriptor m_descriptor = {};
   uint32_t m_poolCount = 0;
   std::aligned_storage<sizeof(control_t), std::alignment_of<control_t>>::value > ::type m_controlData = {};
};

}; // namespace Memory
}; // namespace Foundation
