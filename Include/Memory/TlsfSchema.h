#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <memory.h>
#include <mutex>

#include <Memory/BaseSchema.h>
#include <Util/Assert.h>

#include <EASTL/unique_ptr.h>

#include <tlsf.h>

namespace Foundation
{
namespace Memory
{
template <uint32_t t_pageCount, uint32_t t_pageSize>
class TlsfSchema : public BaseSchema<t_pageCount, t_pageSize>
{
   using TlsfSchemaType = TlsfSchema<t_pageCount, t_pageSize>;

 public:
   static eastl::unique_ptr<TlsfSchema> CreateSchema()
   {
      return eastl::unique_ptr<TlsfSchema>(new TlsfSchemaType());
   }

   TlsfSchema()
   {
      void* memory = malloc(tlsf_size());
      m_tlsf = tlsf_create(memory);
   }

 protected:
   AllocationDescriptor AllocateInternal(uint32_t p_size) final
   {
      std::lock_guard<std::mutex> lock(m_tlsfMutex);

      void* address = tlsf_malloc(m_tlsf, p_size);
      if (!address)
      {
         BaseSchema::PageDescriptor pageDescriptor = AddPage(p_size);
         void* addedAddress = tlsf_malloc(m_tlsf, p_size);
         ASSERT(addedAddress, "Failed to alloate memory with the TLSF schema");

         return AllocationDescriptor{.m_address = addedAddress, .m_size = p_size, .m_pageAddress = pageDescriptor.m_pageAddress};
      }

      return AllocationDescriptor{.m_address = address, .m_size = p_size};
   }

   AllocationDescriptor AllocateAlignedInternal(uint32_t p_size, uint32_t p_alignment, uint32_t p_offset) final
   {
      std::lock_guard<std::mutex> lock(m_tlsfMutex);

      void* address = tlsf_memalign(m_tlsf, p_alignment, p_size);
      if (!address)
      {
         AddPage(p_size);
         void* addedAddress = tlsf_memalign(m_tlsf, p_alignment, p_size);
         ASSERT(addedAddress, "Failed to alloate memory with the TLSF schema");

         return AllocationDescriptor{.m_address = addedAddress, .m_size = p_size, .m_pageAddress = pageDescriptor.m_pageAddress};
      }

      return AllocationDescriptor{.m_address = address, .m_size = p_size};
   }

   void DeallocateInternal(void* p_address, uint32_t p_size) final
   {
      std::lock_guard<std::mutex> lock(m_tlsfMutex);

      tlsf_free(m_tlsf, p_address);
   }

 private:
   BaseSchema::PageDescriptor AddPage(uint32_t p_size)
   {
      // Calculate how many pages need to be allocated
      // const uint32_t pageCount = p_size / m_descriptor.m_pageSize + (p_size % m_descriptor.m_pageSize == 0 ? 0u : 1u);
      const uint32_t pageCount = p_size / t_pageSize + (p_size % t_pageSize == 0 ? 0u : 1u);
      ASSERT(m_pageCount + pageCount <= t_pageCount, "New poolcount will exceed the limit.");

      // Calculate the pool size in bytes
      const uint32_t pageSize = pageCount * t_pageSize;

      // Allocate enough memory for the TlsfPool and a LinkedPool
      uint8_t* pageAddress = (uint8_t*)malloc(pageSize);

      // Add the new pool for tlsf to allocate
      tlsf_add_pool(m_tlsf, pageAddress, pageSize);

      // Create a new page
      PageDescriptor pageDescriptor = {pageAddress, pageSize};
      m_pageDescriptors[m_pageDescriptorIndex++] = pageDescriptor;

      return pageDescriptor;
   }

   tlsf_t m_tlsf = nullptr;
   uint32_t m_pageCount = 0u;

   BaseSchema::PageDescriptor m_pageDescriptors[1024] = {};
   uint32_t m_pageDescriptorIndex = 0u;

   std::mutex m_tlsfMutex;
};

}; // namespace Memory
}; // namespace Foundation
