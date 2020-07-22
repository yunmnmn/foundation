#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <type_traits>

#include <EASTL/unique_ptr.h>

#include <Util/Assert.h>
#include <Util/Util.h>

namespace Foundation
{
namespace Memory
{
// Bootstrap allocator
template <typename t_schema> class BootstrapAllocator
{
   using AllocationDataType = typename std::aligned_storage<sizeof(t_schema), std::alignment_of<t_schema>::value>::type;

 public:
   // EASTL specific functions
   static void* allocate(size_t p_size, int32_t p_flag = 0)
   {
      InitializeSchema();
      return ms_schema->Allocate(p_size);
   }

   static void* allocate(size_t p_size, size_t p_alignment, size_t offset, int flags = 0)
   {
      InitializeSchema();
      return ms_schema->AllocateAligned(p_size, p_alignment, offset);
   }

   static void deallocate(void* p, size_t n)
   {
      ASSERT(ms_schema.get(), "Bootstrap schema isn't initialized");
      ms_schema->Deallocate(p, n);
   }

 private:
   static void InitializeSchema()
   {
      CallOnce(ms_initializedFlag, []() {
         // Create the schema descriptor
         BaseSchema::Descriptor desc = {
             .m_maxPageCount = 1u, .m_pageSize = 1024 * 4u, .m_addPoolCallback = nullptr, .m_removePoolCallback = nullptr};

         // Create the Schema
         ms_schema = t_schema::CreateSchema(desc, (void*)&ms_schemaData);
      });

      ASSERT(ms_schema.get(), "Bootstrap schema isn't initialized");
   }

   static eastl::unique_ptr<t_schema> ms_schema;
   static bool ms_initializedFlag;

   static AllocationDataType ms_schemaData;
};

template <typename t_schema> eastl::unique_ptr<t_schema> BootstrapAllocator<t_schema>::ms_schema;
template <typename t_schema> bool BootstrapAllocator<t_schema>::ms_initializedFlag = false;
template <typename t_schema> typename BootstrapAllocator<t_schema>::AllocationDataType BootstrapAllocator<t_schema>::ms_schemaData;

}; // namespace Memory
}; // namespace Foundation
