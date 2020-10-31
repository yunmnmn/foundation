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
}; // namespace Util

} // namespace Foundation
