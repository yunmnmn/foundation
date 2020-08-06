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
      return Foundation::Memory::StaticClassAllocator<t_class, t_schema>::Get().Allocate(size, #t_class);                          \
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
   static eastl::unique_ptr<BaseAllocator> CreateAllocator(HashName p_name)
   {
      // Create the allocator
      // TODO: does this work for templates?
      auto classAllocator = eastl::make_unique<ClassAllocator<t_class, t_schema>>(p_name);
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
   ClassAllocator(HashName p_name) : BaseAllocator(p_name, eastl::move(t_schema::CreateSchema()))
   {
   }
};

template <typename t_class, typename t_schema>
class StaticClassAllocator
{
 public:
   static void* Allocate(size_t p_size, const char* p_name)
   {
      initializeAllocator(p_name);
      return ms_allocator->Allocate(p_size);
   }

   static void* AllocateAligned(size_t p_size, size_t p_alignment, size_t p_offset, int p_flags = 0)
   {
      initializeAllocator(p_name);
      return ms_allocator->AllocateAllign(p_size, p_alignment);
   }

   static void Deallocate(void* p_address, size_t p_size)
   {
      initializeAllocator(p_name);
      ms_allocator->Deallocate(p_address);
   }

 private:
   static void initializeAllocator(const char* p_name)
   {
      CallOnce(ms_initializedFlag,
               []() { ms_allocator = eastl::move(ClassAllocator<t_class, t_schema>::CreateAllocator(p_name)); });
   }

   static eastl::unique_ptr<BaseAllocator> ms_allocator;
   static bool ms_initializedFlag;
};

}; // namespace Memory
}; // namespace Foundation
//-----------------------------------------------------------------------------
