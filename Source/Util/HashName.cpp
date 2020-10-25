#include <Util/Assert.h>
#include <Util/HashName.h>
#include <Util/Util.h>

#include <EASTL/string.h>

namespace Foundation
{
namespace Util
{
// Define the static members here
std::mutex HashName::ms_hashNameMutex;
bool HashName::ms_initialized = false;

HashName::HashName() : m_Hash(0u)
{
   CallOnce(ms_initialized, [&]() {
      std::lock_guard<std::mutex> lock(ms_hashNameMutex);
      string empty("test");
      GetStringRegistery()[0] = empty;
   });
}

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
      GetStringRegistery()[m_Hash] = p_String;
   }

   m_name = GetStringRegistery()[m_Hash].c_str();
}

HashName::HashName(const char* p_string) : HashName(string(p_string))
{
}

bool HashName::operator==(const HashName& p_Rhs)
{
   return m_Hash == p_Rhs.m_Hash;
}

const char* HashName::GetCStr() const
{
   const auto& it = GetStringRegistery().find(m_Hash);
   ASSERT(it != GetStringRegistery().end(), "String is not set in hash map");

   return it->second.c_str();
}

const uint64_t HashName::Hash() const
{
   return m_Hash;
}

unordered_map<uint64_t, string>& Foundation::Util::HashName::GetStringRegistery()
{
   static unordered_map<uint64_t, string> stringRegistry;
   return stringRegistry;
}
}; // namespace Util
}; // namespace Foundation
