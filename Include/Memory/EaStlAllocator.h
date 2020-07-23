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
class EaStlAllocator : public BaseAllocator
{
   using AllocationDataType = typename std::aligned_storage<sizeof(TlsfSchema), std::alignment_of<TlsfSchema>::value>::type;

 public:
   // EASTL specific functions
   EaStlAllocator(const char* p_name = "EaStlAllocator");
   EaStlAllocator(const EaStlAllocator& p_other);
   EaStlAllocator(const EaStlAllocator& p_other, const char* p_name);

   static void* allocate(size_t p_size, int32_t p_flag = 0);
   static void* allocate(size_t p_size, size_t p_alignment, size_t offset, int flags = 0);
   static void deallocate(void* p, size_t n);

 private:
   static void InitializeSchema();

   static eastl::unique_ptr<TlsfSchema> ms_schema;
   static bool ms_initializedFlag;

   static AllocationDataType ms_schemaData;
};

eastl::unique_ptr<TlsfSchema> EaStlAllocator::ms_schema;
bool EaStlAllocator::ms_initializedFlag = false;
typename EaStlAllocator::AllocationDataType EaStlAllocator::ms_schemaData;

}; // namespace Memory
}; // namespace Foundation
