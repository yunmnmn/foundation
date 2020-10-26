#pragma once

#include <EASTL/unordered_map.h>
#include <Memory/BootstrapAllocator.h>

namespace Foundation
{
namespace Std
{
template <typename t_key, typename t_value>
using unordered_map_bootstrap = eastl::unordered_map<t_key, t_value, eastl::hash<t_key>, eastl::equal_to<t_key>,
                                                     Foundation::Memory::EastlBootstrapAllocator, false>;
};
}; // namespace Foundation
