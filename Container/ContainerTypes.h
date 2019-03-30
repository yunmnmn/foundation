#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <cstring>
#include <algorithm>

#include <Util/Assert.h>

namespace Foundation
{
namespace Container
{
//-----------------------------------------------------------------------------
// TODO: Define default containers with their sizes?
//-----------------------------------------------------------------------------
struct HelperSFINAE
{
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

  template <typename t_Type> static void construct(t_Type* p_Memory)
  {
    if constexpr (std::is_constructible_v<t_Type>)
    {
      p_Memory->~t_Resource();
    }
    else
    {
      memset(p_Memory, 0u, sizeof(t_Type));
    }
  }
};
//-----------------------------------------------------------------------------
}; // namespace Container
}; // namespace Foundation
