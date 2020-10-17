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
namespace bstr
{
using string = eastl::basic_string<char, Memory::BootstrapAllocator<Memory::TlsfSchema<10, 10>>>;

template <typename t_key, typename t_value>
using unordered_map = eastl::unordered_map<t_key, t_value, eastl::hash<t_key>, eastl::equal_to<t_key>,
                                           Memory::BootstrapAllocator<Memory::TlsfSchema>, false>;

}; // namespace bstr

//-----------------------------------------------------------------------------
const uint32_t HashNameMapCapacity = 1024u * 10u;
//-----------------------------------------------------------------------------
struct HashName
{
   HashName();
   HashName(const bstr::string& p_String);
   HashName(const char* p_string);
   HashName(const HashName& p_Rhs) = default;
   bool operator==(const HashName& p_Rhs);

   ~HashName() = default;

   // Get the c_string
   const char* GetCStr() const;

   // Get the HashName's hash
   const uint64_t Hash() const;

 private:
   uint64_t m_Hash = 0u;

   static bstr::unordered_map<uint64_t, bstr::string> ms_StringRegistry;
   static std::mutex ms_hashNameMutex;
   static bool ms_initialized;
};
}; // namespace Foundation
