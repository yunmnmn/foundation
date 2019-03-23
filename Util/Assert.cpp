#include "Assert.h"

#include <iostream>

//-----------------------------------------------------------------------------
void _assert(const char* const p_ExpressionString, const bool p_Expression,
             const char* const p_File, int32_t p_Line,
             const char* const p_Message)
{
  if (!p_Expression)
  {
    std::cerr << "Assert failed:\t" << p_Message << "\n"
              << "Expected:\t" << p_ExpressionString << "\n"
              << "Source:\t\t" << p_File << ", line " << p_Line << "\n";
    abort();
  }
}
//-----------------------------------------------------------------------------
