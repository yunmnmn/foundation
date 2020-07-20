#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <mutex>

#include <EASTL/unique_ptr.h>

#include <Memory/SchemaBase.h>

#include <Util/Assert.h>

namespace Foundation
{
namespace Memory
{
// Bootstrap allocator
template <typename t_schema> class BootstrapAllocator
{
   using AllocationDataType = std::aligned_storage<sizeof(t_schema), std::alignment_of<t_schema>>::value > ::type;

 public:
   static void* allocate(size_t p_size, int32_t p_flag = 0)
   {
      InitializeSchema();
      ms_schema->Allocate(p_size);
   }

   static void* allocate(size_t p_size, size_t p_alignment, size_t offset, int flags = 0)
   {
      InitializeSchema();
      ms_schema->AllocateAligned(p_size, p_alignment, offset);
   }

   static void deallocate(void* p, size_t n)
   {
      ASSERT(ms_schema.get(), "Bootstrap schema isn't initialized");
      ms_schema->Deallocate(p, n);
   }

 private:
   static void InitializeSchema()
   {
      std::call_once(ms_initializedFlag, []() {
         SchemaBase::Descriptor desc = {1u, 1024 * 4u};

         // Create the Schema
         ms_schema = t_schema::CreateSchema(desc, ms_schemaData);
      });

      ASSERT(ms_schema.get(), "Bootstrap schema isn't initialized");
   }

   static eastl::unique_ptr<t_schema> ms_schema;
   static std::once_flag ms_initializedFlag;

   static AllocationDataType ms_schemaData;
};

template <typename t_schema> eastl::unique_ptr<t_schema> BootstrapAllocator<t_schema>::ms_schema;
template <typename t_schema> std::once_flag BootstrapAllocator<t_schema>::ms_initializedFlag;
template <typename t_schema> BootstrapAllocator<t_schema>::AllocationDataType BootstrapAllocator<t_schema>::ms_schemaData;

}; // namespace Memory
}; // namespace Foundation
