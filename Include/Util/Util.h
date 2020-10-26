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

inline Std::string_bootstrap SimpleSprintf(const char* p_Format, ...)
{
   char buffer[1280];
   memset(buffer, 0, 1280);

   va_list argptr;
   va_start(argptr, p_Format);
   uint32_t len = vsnprintf(buffer, 1280, p_Format, argptr);
   va_end(argptr);

   buffer[len] = '\0';

   // return Std::string_bootstrap(buffer);
   return Std::string_bootstrap("");
}

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
}; // namespace Util

} // namespace Foundation
