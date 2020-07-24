#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <memory.h>

#include <Memory/BaseSchema.h>

#include <EASTL/unique_ptr.h>

#include <tlsf.h>

namespace Foundation
{
namespace Memory
{
class BaseAllocator;
class TlsfSchema : public BaseSchema
{
 public:
   static eastl::unique_ptr<BaseSchema> CreateSchema(const BaseSchema::Descriptor& p_desc, BaseAllocator* p_allocator);

   TlsfSchema(const BaseSchema::Descriptor& p_desc, BaseAllocator* p_allocator);

   void* AllocateInternal(uint32_t p_size) final;
   void* AllocateAlignedInternal(uint32_t p_size, uint32_t p_alignment, uint32_t p_offset) final;
   void DeallocateInternal(void* p_address, uint32_t p_size) final;

 private:
   BaseSchema::PageDescriptor AddPageInternal(uint32_t p_size) final;
   void RemovePageInternal(BaseSchema::PageDescriptor& p_pageDescriptor) final;

   tlsf_t m_tlsf = nullptr;
   uint32_t m_pageCount = 0u;

   BaseSchema::PageDescriptor m_pageDescriptors[1024] = {};
   uint32_t m_pageDescriptorIndex = 0u;
};

}; // namespace Memory
}; // namespace Foundation
