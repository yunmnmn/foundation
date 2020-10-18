#include <Util/Assert.h>
#include <Util/HashName.h>
#include <Util/Util.h>

#include <EASTL/string.h>

namespace Foundation
{
//-----------------------------------------------------------------------------
unordered_map<uint64_t, string> HashName::ms_StringRegistry;
std::mutex HashName::ms_hashNameMutex;
bool HashName::ms_initialized = false;
//-----------------------------------------------------------------------------
namespace
{
namespace Internal
{
}; // namespace Internal
}; // namespace
//-----------------------------------------------------------------------------
// HashName
//-----------------------------------------------------------------------------
HashName::HashName() : m_Hash(0u)
{
   CallOnce(ms_initialized, [&]() {
      std::lock_guard<std::mutex> lock(ms_hashNameMutex);
      string empty("test");
      ms_StringRegistry[0] = empty;
   });
}
//-----------------------------------------------------------------------------
HashName::HashName(const string& p_String) : m_Hash(0u)
{
   if (p_String.empty())
      return;

   uint8_t buffer[1024];
   memset(buffer, 0, 1024);
   memcpy(buffer, p_String.data(), p_String.length());

   const uint32_t hashLength = bufferLengthAlign64((uint32_t)p_String.length());
   ASSERT(hashLength < 1024, "That is a long string");

   const uint32_t seed = 1991u;

   MurmurHash3_x64_64(buffer, hashLength, seed, (void*)&m_Hash);

   // TODO: check if entry of the hash is empty
   ASSERT(m_Hash != 0u, "Don't assign the hash at the 0 index");

   {
      std::lock_guard<std::mutex> lock(ms_hashNameMutex);
      ms_StringRegistry[m_Hash] = p_String;
   }
}
//-----------------------------------------------------------------------------
HashName::HashName(const char* p_string) : HashName(string(p_string))
{
}
//-----------------------------------------------------------------------------
bool HashName::operator==(const HashName& p_Rhs)
{
   return m_Hash == p_Rhs.m_Hash;
}
//-----------------------------------------------------------------------------
const char* HashName::GetCStr() const
{
   const auto& it = ms_StringRegistry.find(m_Hash);
   ASSERT(it != ms_StringRegistry.end(), "String is not set in hash map");

   return it->second.c_str();
}
//-----------------------------------------------------------------------------
const uint64_t HashName::Hash() const
{
   return m_Hash;
}
//-----------------------------------------------------------------------------
}; // namespace Foundation
