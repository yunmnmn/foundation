#include <Util/HashName.h>

#include <Util/Util.h>
#include <Memory/Allocator.h>

namespace Foundation
{
//-----------------------------------------------------------------------------
std::unordered_map<uint64_t, std::string> HashName::ms_StringRegistry;
SpinLock HashName::ms_SpinLock;
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
  callOnce([&]() {
    ms_SpinLock.lock();
    std::string empty("");
    ms_StringRegistry[0] = empty;
    ms_SpinLock.unlock();
  });
}
//-----------------------------------------------------------------------------
HashName::HashName(const std::string& p_String) : m_Hash(0u)
{
  if (p_String.empty())
    return;

  uint8_t buffer[1024];
  memset(buffer, 0, 1024);
  memcpy(buffer, p_String.data(), p_String.length());

  const uint32_t hashLength = bufferLengthAlign64((uint32_t)p_String.length());
  ASSERT(hashLength < 1024, "That is a long string");

  // Birth year, yay
  const uint32_t seed = 1991u;

  MurmurHash3_x64_64(buffer, hashLength, seed, (void*)&m_Hash);

  // TODO: check if entry of the hash is empty
  ASSERT(m_Hash != 0u, "Don't assign the hash at the 0 index");

  {
    ms_SpinLock.lock();
    ms_StringRegistry[m_Hash] = p_String;
    m_cstring = ms_StringRegistry[m_Hash].c_str();
    ms_SpinLock.unlock();
  }

  // m_String = p_String;
}
//-----------------------------------------------------------------------------
HashName::HashName(const HashName& p_Rhs)
{
  m_Hash = p_Rhs.m_Hash;
  // m_String = p_Rhs.m_String;
}
//-----------------------------------------------------------------------------
HashName::~HashName()
{
}
//-----------------------------------------------------------------------------
bool HashName::operator==(const HashName& p_Rhs)
{
  return m_Hash == p_Rhs.m_Hash;
}
//-----------------------------------------------------------------------------
const char* HashName::c_str() const
{
  const auto& it = ms_StringRegistry.find(m_Hash);
  ASSERT(it != ms_StringRegistry.end(), "String is not set in hash map");

  return it->second.c_str();
}
//-----------------------------------------------------------------------------
}; // namespace Foundation
