#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <Memory/AllocatorBase.h>

namespace Foundation
{
namespace Memory
{

class TlsfAllocator : public AllocatorBase
{
 public:
   struct Descriptor
   {
      uint32_t m_pageSize = 0u;
   };

   TlsfAllocator(Descriptor& p_desc)
   {
      // Set the members from the descriptor
      m_pageSize = p_desc.m_pageSize;

      void* data = malloc(p_desc.m_pageSize);
      // TODO include eastl and tlsf

      Page& page = AddPage();

      page.m_data =

      // TODO: add page/pool
   }

   void* AllocateInternal(uint32_t p_size)
   {
   }

   void* AllocateAlignInternal(uint32_t p_size, uint32_t p_alignment)
   {
   }

   void DeallocateInternal(void* p_pointer)
   {
   }

   uint32_t m_pageSize = 0u;
};

}; // namespace Memory
}; // namespace Foundation
