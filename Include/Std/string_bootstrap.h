#pragma once

#include <EASTL/string.h>
#include <Memory/BootstrapAllocator.h>

namespace Foundation
{
namespace Std
{
using string_bootstrap = eastl::basic_string<char, Memory::EastlBootstrapAllocator>;
};
}; // namespace Foundation
