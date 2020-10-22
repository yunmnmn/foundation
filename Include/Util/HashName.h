#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <util/Util.h>
#include <Util/MurmurHash3.h>
#include <Util/HashName.h>

#include <Memory/BootstrapAllocator.h>
#include <Memory/TlsfSchema.h>

#include <EASTL/string.h>
#include <EASTL/unordered_map.h>

#include <mutex>

namespace Foundation
{
namespace Util
{
using string = eastl::basic_string<char, Memory::EastlBootstrapAllocator>;

template <typename t_key, typename t_value>
using unordered_map =
    eastl::unordered_map<t_key, t_value, eastl::hash<t_key>, eastl::equal_to<t_key>, Memory::EastlBootstrapAllocator, false>;

//-----------------------------------------------------------------------------
const uint32_t HashNameMapCapacity = 1024u * 10u;
//-----------------------------------------------------------------------------
struct HashName
{
   HashName();
   HashName(const string& p_String);
   HashName(const char* p_string);
   HashName(const HashName& p_Rhs) = default;
   bool operator==(const HashName& p_Rhs);

   ~HashName() = default;

   // Get the c_string
   const char* GetCStr() const;

   // Get the HashName's hash
   const uint64_t Hash() const;

 private:
   // Lazily creates a string registry
   static unordered_map<uint64_t, string>& GetStringRegistery();
   uint64_t m_Hash = 0u;

   static std::mutex ms_hashNameMutex;
   static bool ms_initialized;
};
}; // namespace Util
}; // namespace Foundation
