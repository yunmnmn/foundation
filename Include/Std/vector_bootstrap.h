#pragma once

#include <EASTL/vector.h>
#include <Memory/BootstrapAllocator.h>

namespace Foundation
{
namespace Std
{
template <typename T>
using vector_bootstrap = eastl::vector<T, Foundation::Memory::EastlBootstrapAllocator>;
};
}; // namespace Foundation
