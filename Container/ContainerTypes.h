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

  // template <typename t_Type>
  // constexpr std::enable_if<std::is_constructible_v<t_Type>, void>
  // construct(t_Type* p_Memory)
  //{
  //  // new (p_Memory) t_Resource();
  //}

  // template <typename t_Type>
  // constexpr std::enable_if_t<!std::is_constructible_v<t_Type>, void>
  // construct(t_Type* p_Memory)
  //{
  //  // new (p_Memory) t_Resource();
  //}

  template <typename t_Type> static void construct(t_Type* p_Memory)
  {
    if constexpr (std::is_constructible_v<t_Type>)
      ::new (p_Memory) t_Type;
    else
    {
    }
  }
  //-----------------------------------------------------------------------------
}; // namespace Internal
}; // namespace Container
}; // namespace Foundation
