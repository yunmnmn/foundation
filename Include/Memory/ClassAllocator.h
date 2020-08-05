#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <Util/Assert.h>
#include <Util/Macro.h>

#include <Parallel/SpinLock.h>

#include <Memory/BaseAllocator.h>
#include <Memory/BaseSchema.h>

#include <EASTL/unique_ptr.h>

#define CLASS_ALLOCATOR(t_class, t_schema)                                                                                         \
   inline void* operator new(std::size_t p_size)                                                                                   \
   {                                                                                                                               \
      return Foundation::Memory::StaticClassAllocator<t_class, t_schema>::Get().Allocate(size);                                    \
   }                                                                                                                               \
   inline void operator delete(void* p_address, std::size_t p_size)                                                                \
   {                                                                                                                               \
      if (p_address)                                                                                                               \
      {                                                                                                                            \
         return Foundation::Memory::StaticClassAllocator<t_class, t_schema>::Get().Deallocate(p_address, p_size);                  \
      }                                                                                                                            \
   }

namespace Foundation
{
namespace Memory
{
template <typename t_class, typename t_schema>
class ClassAllocator : public BaseAllocator
{
 public:
   static eastl::unique_ptr<BaseAllocator> CreateAllocator()
   {
      // Create the allocator
      // TODO: does this work for templates?
      auto classAllocator = eastl::make_unique<ClassAllocator<t_class, t_schema>>(typeid(t_class).name());
      return eastl::move(classAllocator);
   }

   void* Allocate(size_t p_size, int32_t p_flag = 0)
   {
      InitializeSchema();
      ms_schema->Allocate(p_size);
   }

   void* AllocateAligned(size_t p_size, size_t p_alignment, size_t offset, int flags = 0)
   {
      InitializeSchema();
      ms_schema->AllocateAligned(p_size, p_alignment, offset);
   }

   void Deallocate(void* p, size_t n)
   {
      ASSERT(ms_schema.get(), "Bootstrap schema isn't initialized");
      ms_schema->Deallocate(p, n);
   }

 private:
   ClassAllocator() : BaseAllocator(#t_class, eastl::move(t_schema::CreateSchema()))
   {
   }
};

template <typename t_class, typename t_schema>
class StaticClassAllocator
{
 public:
   static void* Allocate(size_t p_size, int32_t p_flag = 0)
   {
   }

   static void* AllocateAligned(size_t p_size, size_t p_alignment, size_t p_offset, int p_flags = 0)
   {
   }

   static void Deallocate(void* p_address, size_t p_size)
   {
   }

 private:
   static void initializeAllocator()
   {
      CallOnce(ms_initializedFlag, []() {
         const uint32_t PageCount = 1024u;
         const uint64_t PageSize = 1024 * 1024 * 10u;
         ms_allocator = eastl::move(ClassAllocator<t_class, t_schema>::CreateAllocator("StaticEaStlAllocator"));
      });
   }

   static eastl::unique_ptr<BaseAllocator> ms_allocator;
   static bool ms_initializedFlag;
};

}; // namespace Memory
}; // namespace Foundation
//-----------------------------------------------------------------------------
