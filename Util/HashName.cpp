#include "HashName.h"

#include <Util/Util.h>
#include <Memory/Allocator.h>

namespace Foundation
{
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
    std::string empty("empty");
    ms_StringRegistry.set(0u, empty);
  });
}
//-----------------------------------------------------------------------------
HashName::HashName(const std::string& p_String) : m_Hash(0u)
{
  const static auto bufferLength = [](uint32_t p_Length) -> uint32_t {
    const uint32_t alignment = 8u;
    const uint32_t len =
        (p_Length / alignment) + (p_Length % alignment ? 1u : 0u);
    ASSERT(len, "Should never be 0u");
    return len * alignment;
  };

  static uint8_t buffer[1024];
  memset(buffer, 0, 1024);
  memcpy(buffer, p_String.data(), p_String.length());

  const uint32_t hashLength = bufferLength((uint32_t)p_String.length());
  ASSERT(hashLength < 1024, "That is a long string");

  // Birth year, yay
  const uint32_t seed = 1991u;

  MurmurHash3_x64_64(buffer, hashLength, seed, (void*)&m_Hash);

  // TODO: check if entry of the hash is empty
  ASSERT(m_Hash != 0u, "Don't assign the hash at the 0 index");
  ms_StringRegistry.set(m_Hash, p_String);
}
//-----------------------------------------------------------------------------
HashName::HashName(const HashName& p_Rhs)
{
  m_Hash = p_Rhs.m_Hash;
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
  // TODO: assert if it's valid or not
  std::string defaultString("not Found");
  defaultString = ms_StringRegistry.get(m_Hash, defaultString);
  return defaultString.c_str();
}
//-----------------------------------------------------------------------------
Foundation::Container::SimpleFixedMap<
    Foundation::Container::DefaultContainerAllocatorInterface, std::string,
    HashNameMapCapacity>
    HashName::ms_StringRegistry;
//-----------------------------------------------------------------------------
}; // namespace Foundation
