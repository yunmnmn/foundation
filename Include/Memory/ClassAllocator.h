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
      return Foundation::Memory::AllocatorInterface<_Class, _Allocator, _DescriptorFunctor>::Get().Allocate(                       \
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
class ClassAllocator : public AllocatorBase
{
   static constexpr uint32_t InvalidElementIndex = static_cast<uint32_t>(-1);

 public:
   struct Descriptor
   {
      uint32_t m_pageCapacity = 0u;
      uint32_t m_elementSize = 0u;
   };

   NO_COPY_ASSIGN(PoolAllocator);

   void Grow()
   {
   }

   PoolAllocator(const Descriptor& p_descriptor)
   {
   }

   ~PoolAllocator()
   {
   }

   void* Allocate()
   {
      // Pool is already full
      if (m_poolIndex >= PoolSize)
      {
         while (m_flag.test_and_set())
            ;

         m_flag.clear();
      }
      else
      {
         // If it's not locket yet
         if (!m_flag.test_and_set() && m_freeIndex != InvalidElementIndex)
         {
            ElementType* freeObject = &m_objects[m_freeIndex];

            // Set the next free index
            const uint32_t previousFreeIndex = m_freeIndex;
            m_freeIndex = m_nextIndex[m_freeIndex];

            // Set the current one
            m_nextIndex[previousFreeIndex] = InvalidElementIndex;

            // Re-use an object
            m_flag.clear();
            return freeObject;
         }

         // If it's already locked, get a new one
         m_poolIndex++;
      }
   }

   void* AllocateAllign()
   {
   }

   void Deaclloate(void* memory)
   {
   }

 private:
   std::atomic_uint32_t m_poolIndex = 0u;
   uint32_t m_freeIndex = InvalidElementIndex;

   ElementType m_objects[PoolSize];
   uint32_t m_nextIndex[PoolSize] = {InvalidElementIndex};

   std::atomic_flag m_flag;
   SpinLock m_spinLock;
};
}; // namespace Memory
}; // namespace Foundation
//-----------------------------------------------------------------------------
