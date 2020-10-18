#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <Util/Assert.h>
#include <Util/Macro.h>

#include <Parallel/SpinLock.h>

#include <Memory/BaseAllocator.h>
#include <Memory/BaseSchema.h>

#include <EASTL/unique_ptr.h>

#define CLASS_ALLOCATOR(t_class)                                                                                                   \
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
template <typename t_allocator>
class StaticAllocatorWrapper
{
   using AllocatorDataType = typename std::aligned_storage<sizeof(t_allocator), std::alignof(t_allocator)>::type;

 public:
   static void* allocate(size_t p_size, int32_t p_flag = 0)
   {
      initializeAllocator(p_name);
      return ms_allocator->Allocate(p_size);
   }

   static void* allocate(size_t p_size, size_t p_alignment, size_t p_offset, int p_flags = 0)
   {
      initializeAllocator(p_name);
      return ms_allocator->AllocateAllign(p_size, p_alignment);
   }

   static void deallocate(void* p_address, size_t p_size)
   {
      ASSERT(ms_allocator.get() != nullptr, "Allocator isn't initalized yet");
      ms_allocator->Deallocate(p_address);
   }

 private:
   static void initializeAllocator(const char* p_name)
   {
      // TODO: Replace this with a double checked
      // TODO: placement new to create an instance of the allocator
      CallOnce(ms_initializedFlag,
               []() { ms_allocator = eastl::move(StaticAllocatorWrapper<t_class, t_schema>::CreateAllocator(p_name)); });
   }

   static eastl::unique_ptr<t_allocator> ms_allocator;
   static bool ms_initializedFlag;
   static StaticAllocatorWrapper ms_allocatorData = {};
};
template <typename t_allocator>
eastl::unique_ptr<t_allocator> StaticAllocatorWrapper<t_allocator>::ms_allocator;
template <typename t_allocator>
bool StaticAllocatorWrapper<t_allocator>::ms_initializedFlag = false;
template <typename t_allocator>
StaticAllocatorWrapper<t_allocator>::AllocatorDataType StaticAllocatorWrapper<t_allocator>::ms_allocatorData;

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

template <typename t_allocator>
class StaticEastlAllocatorWrapper
{
   using AllocatorDataType = typename std::aligned_storage<sizeof(t_allocator), std::alignof(t_allocator)>::type;

 public:
   StaticEastlAllocatorWrapper(const char* p_name)
   {
   }

   StaticEastlAllocatorWrapper(const StaticEastlAllocatorWrapper& p_other)
   {
   }

   StaticEastlAllocatorWrapper(const StaticEastlAllocatorWrapper& p_other, const char* p_name)
   {
   }

   bool operator!=(const BootstrapAllocator& other)
   {
      // Only one bootstrap allocator, so always return true
      return true;
   }

   static void* allocate(size_t p_size, int32_t p_flag = 0)
   {
      initializeAllocator(p_name);
      return m_allocator->Allocate(p_size);
   }

   static void* allocate(size_t p_size, size_t p_alignment, size_t p_offset, int p_flags = 0)
   {
      initializeAllocator(p_name);
      return m_allocator->AllocateAllign(p_size, p_alignment);
   }

   static void deallocate(void* p_address, size_t p_size)
   {
      initializeAllocator(p_name);
      m_allocator->Deallocate(p_address, p_size);
   }

 private:
   static void initializeAllocator(const char* p_name)
   {
      // TODO: Replace this with a double checked
      // TODO: placement new to create an instance of the allocator
      CallOnce(ms_initializedFlag,
               []() { ms_allocator = eastl::move(StaticEastlAllocatorWrapper<t_class, t_schema>::CreateAllocator(p_name)); });
   }

   static eastl::unique_ptr<t_allocator> ms_allocator;
   static bool ms_initializedFlag;
   static StaticAllocatorWrapper ms_allocatorData = {};
};
template <typename t_allocator>
eastl::unique_ptr<t_allocator> StaticEastlAllocatorWrapper<t_allocator>::ms_allocator;
template <typename t_allocator>
bool StaticEastlAllocatorWrapper<t_allocator>::ms_initializedFlag = false;
template <typename t_allocator>
StaticEastlAllocatorWrapper<t_allocator>::AllocatorDataType StaticEastlAllocatorWrapper<t_allocator>::ms_allocatorData;

}; // namespace Memory
}; // namespace Foundation
//-----------------------------------------------------------------------------
