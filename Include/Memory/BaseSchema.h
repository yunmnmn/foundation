#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <memory>

namespace Foundation
{
namespace Memory
{
struct SchemaDescriptor
{
   // Limit amount of pages the schema can create
   uint32_t m_maxPageCount = 0u;

   // Size of a page in bytes
   uint64_t m_pageSize = 0u;
};

struct PageDescriptor
{
   // Starting address of the page
   void* m_pageAddress = nullptr;

   // Page size
   uint64_t m_pageSize = 0u;
};

struct AllocationDescriptor
{
   // Address where the memory is allocated
   void* m_address = nullptr;

   // Allocation size
   uint64_t m_size;

   // Starting address of the page where the allocation is
   void* m_pageAddress = nullptr;
};

template <uint32_t t_pageCount, uint64_t t_pageSize>
class BaseSchema
{
 public:
   BaseSchema() = default;

   AllocationDescriptor Allocate(uint64_t p_size)
   {
      return AllocateInternal(p_size);
   }

   AllocationDescriptor AllocateAligned(uint64_t p_size, uint32_t p_alignment, uint64_t p_offset)
   {
      return AllocateAlignedInternal(p_size, p_alignment, p_offset);
   }

   void Deallocate(void* p_address, uint64_t p_size)
   {
      DeallocateInternal(p_address, p_size);
   }

 protected:
   // Get page count
   constexpr uint32_t GetPageCount() const
   {
      return t_pageCount;
   }

   // Get page size in bytes
   constexpr uint64_t GetPageSize() const
   {
      return t_pageSize;
   }

 private:
   virtual AllocationDescriptor AllocateInternal(uint64_t p_size) = 0;
   virtual AllocationDescriptor AllocateAlignedInternal(uint64_t p_size, uint32_t p_alignment, uint64_t p_offset) = 0;
   virtual void DeallocateInternal(void* p_address, uint64_t p_size) = 0;
};

}; // namespace Memory
}; // namespace Foundation
