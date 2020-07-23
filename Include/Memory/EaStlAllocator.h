#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <type_traits>

#include <EASTL/unique_ptr.h>

#include <Memory/BaseAllocator.h>
#include <Memory/BaseSchema.h>
#include <Memory/TlsfSchema.h>

namespace Foundation
{
namespace Memory
{
// TlsfAllocator
class EaStlAllocator : public BaseAllocator<EaStlAllocator, TlsfSchema>
{
 public:
   // EASTL specific functions
   EaStlAllocator(const char* p_name = "EaStlAllocator");
   EaStlAllocator(const EaStlAllocator& p_other);
   EaStlAllocator(const EaStlAllocator& p_other, const char* p_name);

   void* allocate(size_t p_size, int32_t p_flag = 0);
   void* allocate(size_t p_size, size_t p_alignment, size_t offset, int flags = 0);
   void deallocate(void* p, size_t n);

 private:
   void* AllocateInternal(uint32_t p_size) final;
   void* AllocateAlignInternal(uint32_t p_size, uint32_t p_alignment) final;
   void DeallocateInternal(void* p_pointer) final;

   BaseSchema::Descriptor GetDescriptor();
};

}; // namespace Memory
}; // namespace Foundation
