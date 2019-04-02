#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <cstring>
#include <algorithm>

#include <Util/Assert.h>
#include <Memory/Allocator.h>

namespace Foundation
{
namespace Container
{
//-----------------------------------------------------------------------------
struct Allocators
{
  static Foundation::Memory::TlsfAllocator ms_DynamicResourceAllocator;
};
//-----------------------------------------------------------------------------
class DefaultContainerAllocatorInterface
{
public:
  DefaultContainerAllocatorInterface() = default;
  ~DefaultContainerAllocatorInterface() = default;

  static void* allocate(const uint64_t p_SizeInBytes)
  {
    return Foundation::Container::Allocators::ms_DynamicResourceAllocator
        .allocate(p_SizeInBytes, 0);
  }

  static void* allocateAligned(const uint64_t p_SizeInBytes,
                               const uint32_t p_Allignment,
                               const uint32_t p_Offset)
  {
    return Foundation::Container::Allocators::ms_DynamicResourceAllocator
        .allocateAligned(p_SizeInBytes, p_Allignment, p_Offset, 0);
  }

  static void free(void* p_Memory, uint64_t p_SizeInBytes)
  {
    return Foundation::Container::Allocators::ms_DynamicResourceAllocator.free(
        p_Memory, p_SizeInBytes);
  }
};
//-----------------------------------------------------------------------------
// TODO: Define default containers with their sizes?
//-----------------------------------------------------------------------------
const uint32_t Empty = (uint32_t)-1;
//-----------------------------------------------------------------------------
struct HelperSFINAE
{
  //-----------------------------------------------------------------------------
  template <typename t_Type>
  static void copy(t_Type* const p_Begin, t_Type* const p_End,
                   t_Type* const p_Dest)
  {
    if constexpr (std::is_pod_v<t_Type>)
    {
      const uint64_t test = p_End - p_Begin;
      memcpy((void*)p_Dest, (void*)p_Begin, test);
    }
    else
    {
      t_Type* mem = p_Begin;
      while (mem != p_End)
      {
        *p_Dest = *mem;
        mem++;
      }
    }
  }
  //-----------------------------------------------------------------------------
  template <typename t_Type> static void construct(t_Type* p_Memory)
  {
    if constexpr (std::is_constructible_v<t_Type>)
    {
      ::new (p_Memory) t_Type();
    }
    else
    {
      memset(p_Memory, 0u, sizeof(t_Type));
    }
  }
  //-----------------------------------------------------------------------------
  template <typename t_Type>
  static void constructRanged(t_Type* p_MemoryBegin, t_Type* p_MemoryEnd)
  {
    if constexpr (std::is_constructible_v<t_Type>)
    {
      for (t_Type* memory = p_MemoryBegin; p_MemoryBegin != p_MemoryEnd;
           memory++)
      {
        ::new (memory) t_Type();
      }
    }
    else
    {
      memset(p_MemoryBegin, 0u, sizeof(t_Type) * (p_MemoryEnd - p_MemoryBegin));
    }
  }
  //-----------------------------------------------------------------------------
  template <typename t_Type> static void destruct(t_Type* p_Memory)
  {
    if constexpr (std::is_constructible_v<t_Type>)
    {
      p_Memory->~t_Type();
    }
    else
    {
      memset(p_Memory, 0u, sizeof(t_Type));
    }
  }
  //-----------------------------------------------------------------------------
  template <typename t_Type>
  static void destructRanged(t_Type* p_MemoryBegin, t_Type* p_MemoryEnd)
  {
    if constexpr (std::is_constructible_v<t_Type>)
    {
      for (t_Type* memory = p_MemoryBegin; p_MemoryBegin != p_MemoryEnd;
           memory++)
      {
        memory->~t_Type();
      }
    }
    else
    {
      memset(p_MemoryBegin, 0u, sizeof(t_Type) * (p_MemoryEnd - p_MemoryBegin));
    }
  }
  //-----------------------------------------------------------------------------
};
//-----------------------------------------------------------------------------
}; // namespace Container
}; // namespace Foundation
