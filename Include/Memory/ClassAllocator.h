#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <Memory/BaseAllocator.h>
#include <Memory/StaticAllocatorWrapper.h>
#include <Memory/PoolSchema.h>

#define CLASS_ALLOCATOR_PAGECOUNT_PAGESIZE(CLASS, PAGECOUNT, PAGESIZE)                                                             \
   static constexpr char ClassName[] = #CLASS;                                                                                     \
   inline void* operator new(std::size_t p_size)                                                                                   \
   {                                                                                                                               \
      using namespace Foundation::Memory;                                                                                          \
      using ClassSchema = PoolSchema<CLASS, PAGECOUNT, PAGESIZE>;                                                                  \
      using ClassAllocator = Foundation::Memory::StaticAllocatorWrapper<DefaultAllocator<ClassName, ClassSchema>>;                 \
      return ClassAllocator::Allocate(p_size);                                                                                     \
   }                                                                                                                               \
   inline void operator delete(void* p_address, std::size_t p_size)                                                                \
   {                                                                                                                               \
      using namespace Foundation::Memory;                                                                                          \
      using ClassSchema = PoolSchema<CLASS, PAGECOUNT, PAGESIZE>;                                                                  \
      using ClassAllocator = StaticAllocatorWrapper<DefaultAllocator<ClassName, ClassSchema>>;                                     \
      if (p_address)                                                                                                               \
      {                                                                                                                            \
         ClassAllocator::Deallocate(p_address, p_size);                                                                            \
      }                                                                                                                            \
   }                                                                                                                               \
   inline void* operator new[]([[maybe_unused]] size_t p_size)                                                                     \
   {                                                                                                                               \
      return nullptr;                                                                                                              \
   }
