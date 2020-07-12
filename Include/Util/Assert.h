#pragma once

#include <inttypes.h>
#include <stdbool.h>

void _assert(const char* const p_ExpressionString, const bool p_Expression,
             const char* const p_File, int32_t p_Line,
             const char* const p_Message);

#ifndef NDEBUG
#define ASSERT(Expr, Msg) _assert(#Expr, Expr, __FILE__, __LINE__, Msg)
#define STATIC_ASSERT(Expr, Msg)                                               \
  _staticAssert(#Expr, Expr, __FILE__, __LINE__, Msg)
#else
#define ASSERT(Expr, Msg) ;
#define STATIC_ASSERT(Expr, Msg) ;
#endif
