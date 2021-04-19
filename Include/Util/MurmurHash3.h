//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#ifndef _MURMURHASH3_H_
#define _MURMURHASH3_H_

// To handle 64-bit data; see https://docs.python.org/2.7/c-api/arg.html
#ifndef PY_SSIZE_T_CLEAN
#define PY_SSIZE_T_CLEAN
#endif

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#if defined(_MSC_VER)
typedef unsigned __int8 uint8_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;

// Other compilers

#else // defined(_MSC_VER)

#include <stdint.h>

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------
uint32_t bufferLengthAlign64(uint32_t p_Size);

void MurmurHash3_x86_32(const void* key, int len, uint32_t seed, void* out);

void MurmurHash3_x86_128(const void* key, int len, uint32_t seed, void* out);

void MurmurHash3_x64_128(const void* key, int len, uint32_t seed, void* out);

void MurmurHash3_x64_64(const void* key, int len, uint32_t seed, void* out);

template <typename t_type>
uint64_t MurmurHash3_x64_64_Helper(const t_type* key)
{
   const uint32_t hashLength = bufferLengthAlign64(sizeof(t_type));
   ASSERT(hashLength < 1024, "Type is too big");

   const uint32_t seed = 1991u;

   uint64_t hash = 0u;
   MurmurHash3_x64_64(key, hashLength, seed, (void*)&hash);

   return hash;
}
//-----------------------------------------------------------------------------
// Align to 8 bytes (64 bits)
uint32_t bufferLengthAlign64(uint32_t p_Size);
//-----------------------------------------------------------------------------
#endif // _MURMURHASH3_H_
