#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <Memory/BootstrapAllocator.h>

// TODO delete
#include <vector>

namespace Foundation
{
namespace Memory
{
struct AllocatorPage
{
   // Page size in bytes
   uint32_t m_pageSize = 0u;
   void* m_data = nullptr;
};

class AllocatorBase
{
 public:
   AllocatorBase()
   {
   }

   virtual void* AllocateInternal(uint32_t p_size) = 0;
   virtual void* AllocateAlignInternal(uint32_t p_size, uint32_t p_alignment) = 0;
   virtual void DeallocateInternal(void* p_pointer) = 0;

   uint32_t GetPageCount() const
   {
      return static_cast<uint32_t>(m_pageArray.size());
   }

   void* Allocate(uint32_t p_size)
   {
   }

   void* AllocateAllign(uint32_t p_size, uint32_t p_alignment)
   {
   }

   void* Deallocate(void* p_pointer)
   {
   }

   AllocatorPage& AddPage()
   {
      m_pageArray.emplace_back();
      return m_pageArray.back();
   }

   std::vector<AllocatorPage> m_pageArray;
};

}; // namespace Memory
}; // namespace Foundation
