#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <atomic>

namespace Foundation
{
//-----------------------------------------------------------------------------
template <typename Functor>
void CallOnce(bool& called, Functor functor)
{
   if (!called)
   {
      called = true;
      functor();
   }
}
//---------------------------------------------------------------------------------//
inline std::string simpleSprintf(const char* p_Format, ...)
{
   char buffer[1280];
   memset(buffer, 0, 1280);

   va_list argptr;
   va_start(argptr, p_Format);
   uint32_t len = vsnprintf(buffer, 1280, p_Format, argptr);
   va_end(argptr);

   buffer[len] = '\0';

   return std::string(buffer);
}
//-----------------------------------------------------------------------------
// Simple Linear congruential generator
class LcgRandom
{
   uint32_t state;

 public:
   LcgRandom() : state(2578432553)
   {
   }

   void SetSeed(uint32_t seed)
   {
      state = seed;
   }

   uint16_t Get()
   {
      state = 214013 * state + 2531011;
      uint16_t rnd = (state >> 16);
      return rnd;
   }
};
//-----------------------------------------------------------------------------
} // namespace Foundation
