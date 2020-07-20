#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <Util/Assert.h>
#include <Util/Macro.h>
#include <Parallel/SpinLock.h>
#include <Memory/AllocatorBase.h>

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
template <typename t_class, typename t_schema> class ClassAllocator
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
      std::call_once(ms_initializedFlag, []() {
         SchemaBase::Descriptor desc = {1u, 1024 * 4u};

         // Create the Schema
         ms_schema = t_schema::CreateSchema(desc, ms_schemaData);
      });

      ASSERT(ms_schema.get(), "Bootstrap schema isn't initialized");
   }

   static void Allocate()
   {
      Get()->Allocate();
   }

   static void Deallocate(void* data, uint32_t size)
   {
   }

   t_allocator* ms_allocator = nullptr;
   static std::aligned_storage<sizeof(t_allocator), std::alignment_of<t_allocato>>::value > ::type ms_allocatorData = {};
};
}; // namespace Memory
}; // namespace Foundation
//-----------------------------------------------------------------------------
