#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <memory>

namespace Foundation
{
namespace Memory
{
class BaseAllocator;

class BaseSchema
{
 public:
   struct Descriptor
   {
      uint32_t m_maxPageCount = 0u;
      uint32_t m_pageSize = 0u;
   };

   struct PageDescriptor
   {
      void* m_pageAddress;
      uint32_t m_pageSize;
   };

   void* Allocate(uint32_t p_size);
   void* AllocateAligned(uint32_t p_size, uint32_t p_alignment, uint32_t p_offset);
   void Deallocate(void* p_address, uint32_t p_size);

 protected:
   BaseSchema(const Descriptor& desc, BaseAllocator* p_allocator);

   void AddPage(uint32_t p_size);
   void RemovePage(PageDescriptor& p_pageDescriptor);

   Descriptor m_descriptor = {};

 private:
   virtual void* AllocateInternal(uint32_t p_size) = 0;
   virtual void* AllocateAlignedInternal(uint32_t p_size, uint32_t p_alignment, uint32_t p_offset) = 0;
   virtual void DeallocateInternal(void* p_address, uint32_t p_size) = 0;

   virtual PageDescriptor AddPageInternal(uint32_t p_size) = 0;
   virtual void RemovePageInternal(PageDescriptor& m_pageDescriptor) = 0;

   BaseAllocator* m_allocator = nullptr;
};

}; // namespace Memory
}; // namespace Foundation
