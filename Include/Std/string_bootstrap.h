#pragma once

#include <EASTL/string.h>
#include <Memory/BootstrapAllocator.h>

namespace Foundation
{
namespace Std
{
using string_bootstrap = eastl::basic_string<char, Memory::EastlBootstrapAllocator>;
using wstring_bootstrap = eastl::basic_string<wchar_t, Memory::EastlBootstrapAllocator>;
}; // namespace Std
}; // namespace Foundation
