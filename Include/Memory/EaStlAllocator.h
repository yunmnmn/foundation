#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <type_traits>

#include <EASTL/unique_ptr.h>

#include <Memory/BaseAllocator.h>
#include <Memory/TlsfSchema.h>

namespace Foundation
{
namespace Memory
{
// TlsfAllocator
template <uint32_t t_pageCount, uint64_t t_pageSize> class EaStlAllocator : public BaseAllocator
{
   // TODO: overload the new and delete of this
 public:
   static eastl::unique_ptr<BaseAllocator> CreateAllocator(HashName p_name)
   {
      // Create the allocator
      auto eastlAllocator = eastl::make_unique<EaStlAllocator>(p_name.c_str());

      return eastl::move(eastlAllocator);
   }

   // EASTL specific functions
   EaStlAllocator(const char* p_name = "EaStlAllocator")
       : BaseAllocator(p_name, eastl::move(TlsfSchema::CreateSchema({t_pageCount, t_pageSize}, this)))
   {
   }

   EaStlAllocator(const EaStlAllocator& p_other)
   {
      // TODO
   }

   EaStlAllocator(const EaStlAllocator& p_other, const char* p_name)
   {
      // TODO
   }

   void* allocate(size_t p_size, int32_t p_flag = 0)
   {
      return Allocate(p_size);
   }

   void* allocate(size_t p_size, size_t p_alignment, size_t offset, int flags = 0)
   {
      return AllocateAllign(p_size, p_alignment);
   }

   void deallocate(void* p_address, size_t p_size)
   {
      Deallocate(p_address, p_size);
   }

 private:
   void* AllocateInternal(uint64_t p_size) final
   {
      return m_schema->Allocate(p_size);
   }

   void* AllocateAlignInternal(uint64_t p_size, uint32_t p_alignment) final
   {
      // TODO FIx the parameters
      return m_schema->AllocateAligned(p_size, p_alignment, 0);
   }

   void DeallocateInternal(void* p_pointer, uint64_t p_size) final
   {
      // TODO FIx the parameters
      m_schema->Deallocate(p_pointer, 0);
   }
};

}; // namespace Memory
}; // namespace Foundation
