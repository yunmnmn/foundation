#pragma once

#include <unordered_map>
#include <inttypes.h>
#include <stdbool.h>
#include <string>

#include <util/Util.h>
#include <Util/MurmurHash3.h>
#include <Util/HashName.h>

#include <Memory/BootstrapAllocator.h>

#include <EASTL/string.h>

namespace Foundation
{
//-----------------------------------------------------------------------------
const uint32_t HashNameMapCapacity = 1024u * 10u;
//-----------------------------------------------------------------------------
struct HashName
{
   HashName();
   HashName(const eastl::string& p_String);
   HashName(const HashName& p_Rhs);
   ~HashName();

   bool operator==(const HashName& p_Rhs);
   const char* c_str() const;

   const uint64_t hash() const
   {
      return m_Hash;
   }

 private:
   uint64_t m_Hash;
   const char* m_cstring;

   static std::unordered_map<uint64_t, std::string> ms_StringRegistry;
   static SpinLock ms_SpinLock;
   static bool ms_initialized;
};
}; // namespace Foundation
