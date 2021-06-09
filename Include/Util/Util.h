#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <Std/string_bootstrap.h>

namespace Foundation
{
namespace Util
{
template <typename Functor>
void CallOnce(bool& called, Functor functor)
{
   if (!called)
   {
      called = true;
      functor();
   }
}

template <typename t_stringType>
inline t_stringType SimpleSprintf(const char* p_Format, ...)
{
   char buffer[1280];
   memset(buffer, 0, 1280);

   va_list argptr;
   va_start(argptr, p_Format);
   uint32_t len = vsnprintf(buffer, 1280, p_Format, argptr);
   va_end(argptr);

   buffer[len] = '\0';

   return t_stringType(buffer);
}

// Helper function to convert custom flags bits to native flag bits
template <typename t_nativeFlagBits, typename t_map, typename t_customFlagBits>
static t_nativeFlagBits FlagsToNativeHelper(const t_map& p_map, t_customFlagBits p_flags)
{
   // TODO: Statically check if it's all 32 bit flags
   uint32_t returnBits = 0u;
   for (uint32_t i = 0u; i < 32u; i++)
   {
      const uint32_t currentBit = (1 << i);
      if (currentBit & static_cast<uint32_t>(p_flags))
      {
         const auto& mapIt = p_map.find(static_cast<t_customFlagBits>(currentBit));
         ASSERT(mapIt != p_map.end(), "Flag conversion to Vulkan doesn't exist");

         returnBits |= static_cast<uint32_t>(mapIt->second);
      }
   }

   return static_cast<t_nativeFlagBits>(returnBits);
}

// Helper function to convert custom enum to native enum
template <typename t_nativeEnum, typename t_map, typename t_rendererEnum>
static t_nativeEnum EnumToNativeHelper(const t_map& p_map, t_rendererEnum p_flags)
{
   const auto& mapIt = p_map.find(p_flags);
   ASSERT(mapIt != p_map.end(), "Enum conversion doesn't exist");

   return mapIt->second;
}

template <typename... t_args>
static uint32_t SetFlagsInternal(t_args... p_args)
{
   return (... | static_cast<uint32_t>(p_args));
}

template <typename t_enumType, typename... t_args>
static t_enumType SetFlags(t_args... p_args)
{
   return static_cast<t_enumType>(SetFlagsInternal(p_args...));
}

}; // namespace Util

} // namespace Foundation
