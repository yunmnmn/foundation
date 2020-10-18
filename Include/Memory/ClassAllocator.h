#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <Util/Assert.h>
#include <Util/Macro.h>

#include <Memory/BaseAllocator.h>
#include <Memory/BaseSchema.h>

#include <Memory/PoolSchema.h>

#define CLASS_ALLOCATOR_PAGECOUNT_PAGESIZE(CLASS, PAGECOUNT, PAGESIZE)                                                             \
   inline void* operator new(std::size_t p_size)                                                                                   \
   {                                                                                                                               \
      using ClassSchema = Foundation::Memory::PoolSchema<CLASS, PAGECOUNT, PAGESIZE>;                                              \
      using ClassAllocator = Foundation::Memory::StaticAllocatorWrapper<DefaultAllocator<#CLASS, ClassSchema>>;                    \
      return ClassAllocator::Allocate(p_size);                                                                                     \
   }                                                                                                                               \
   inline void operator delete(void* p_address, std::size_t p_size)                                                                \
   {                                                                                                                               \
      using ClassSchema = Foundation::Memory::PoolSchema<CLASS, PAGECOUNT, PAGESIZE>;                                              \
      using ClassAllocator = Foundation::Memory::StaticAllocatorWrapper<DefaultAllocator<#CLASS, ClassSchema>>;                    \
      return ClassAllocator::Allocate(p_size);                                                                                     \
      if (p_address)                                                                                                               \
      {                                                                                                                            \
         ClassAllocator::Deallocate(p_address, p_size);                                                                            \
      }                                                                                                                            \
   }

namespace Foundation
{
namespace Memory
{
template <typename t_allocator>
class StaticAllocatorWrapper
{
   using AllocatorDataType = typename std::aligned_storage<sizeof(t_allocator), std::alignof(t_allocator)>::type;

 public:
   static void* Allocate(size_t p_size)
   {
      return GetAllocator().Allocate(p_size);
   }

   static void* AllocateAllign(size_t p_size, size_t p_alignment)
   {
      return GetAllocator().AllocateAllign(p_size, p_alignment);
   }

   static void Deallocate(void* p_address, size_t p_size)
   {
      GetAllocator().Deallocate(p_address);
   }

 private:
   t_allocator& GetAllocator()
   {
      static t_allocator allocator;
      return allocator;
   }
};

template <typename t_allocator>
class EastlAllocatorWrapper
{
 public:
   EastlAllocatorWrapper(const char* p_name)
   {
   }

   EastlAllocatorWrapper(const EastlAllocatorWrapper& p_other)
   {
   }

   EastlAllocatorWrapper(const EastlAllocatorWrapper& p_other, const char* p_name)
   {
   }

   bool operator!=(const BootstrapAllocator& other)
   {
      // Only one bootstrap allocator, so always return true
      return true;
   }

   void* allocate(size_t p_size, int32_t p_flag = 0)
   {
      m_allocator.Allocate(p_size);
   }

   void* allocate(size_t p_size, size_t p_alignment, size_t p_offset, int p_flags = 0)
   {
      m_allocator.AllocateAllign(p_size, p_alignment);
   }

   void deallocate(void* p_address, size_t p_size)
   {
      Deallocate(p_address, p_size);
   }

 private:
   t_allocator m_allocator;
};

}; // namespace Memory
}; // namespace Foundation
//-----------------------------------------------------------------------------
