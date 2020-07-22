#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <Util/Assert.h>
#include <Util/Macro.h>

#include <Parallel/SpinLock.h>

#include <Memory/AllocatorBase.h>
#include <Memory/SchemaBase.h>

#define CLASS_ALLOCATOR(_Class, _Allocator)                                                                                        \
   inline void* operator new(std::size_t size)                                                                                     \
   {                                                                                                                               \
      return Foundation::Memory::ClassAllocator<_Class, _Allocator, _DescriptorFunctor>::Get().Allocate(                           \
          size, std::alignment_of<_Class>::value, #_Class);                                                                        \
   }                                                                                                                               \
   inline void operator delete(void* p, std::size_t size)                                                                          \
   { /* default operator delete */                                                                                                 \
      if (p)                                                                                                                       \
      {                                                                                                                            \
         AZ::AllocatorInstance<_Allocator>::Get().DeAllocate(p, size, AZStd::alignment_of<_Class>::value);                         \
      }                                                                                                                            \
   }

namespace Foundation
{
namespace Memory
{
template <typename t_class, typename t_schema> class ClassAllocator : public AllocatorBase
{
 public:
   static void* Allocate(size_t p_size, int32_t p_flag = 0)
   {
      InitializeSchema();
      ms_schema->Allocate(p_size);
   }

   static void* AllocateAligned(size_t p_size, size_t p_alignment, size_t offset, int flags = 0)
   {
      InitializeSchema();
      ms_schema->AllocateAligned(p_size, p_alignment, offset);
   }

   static void Deallocate(void* p, size_t n)
   {
      ASSERT(ms_schema.get(), "Bootstrap schema isn't initialized");
      ms_schema->Deallocate(p, n);
   }

 private:
   static void InitializeSchema()
   {
      CallOnce(ms_initializedFlag, []() {
         // Create the schema descriptor
         SchemaBase::Descriptor desc = {.m_maxPageCount = 1024u, .m_pageSize = 1024 * sizeof(t_class)};

         // Create the Schema
         ms_schema = t_schema::CreateSchema(desc, ms_schemaData);
      });

      ASSERT(ms_schema.get(), "Bootstrap schema isn't initialized");
   }

   template <typename t_schema> bool BootstrapAllocator<t_schema>::ms_initializedFlag = false;
   static std::aligned_storage<sizeof(t_allocator), std::alignment_of<t_allocato>>::value > ::type ms_allocatorData = {};
};
}; // namespace Memory
}; // namespace Foundation
//-----------------------------------------------------------------------------
