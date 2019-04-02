#include "HashName.h"

#include <Util/Util.h>
#include <Memory/Allocator.h>

namespace Foundation
{
namespace
{
namespace Internal
{
//-----------------------------------------------------------------------------
const uint64_t KiloByte = sizeof(uint8_t) * 1024u;
const uint64_t MegaByte = KiloByte * 1024u;
const uint64_t HashNameMemoryCapacity = MegaByte;
//-----------------------------------------------------------------------------
//// Allocate all the resources here
//::Foundation::Memory::LinearAllocator<false>
//    g_ResourceAllocator(HashNameMemoryCapacity);
//-----------------------------------------------------------------------------
}; // namespace Internal
}; // namespace
//-----------------------------------------------------------------------------
// HashName
//-----------------------------------------------------------------------------
// HashNameAllocatorInterface::HashNameAllocatorInterface()
//{
//}
////-----------------------------------------------------------------------------
// HashNameAllocatorInterface::~HashNameAllocatorInterface()
//{
//}
////-----------------------------------------------------------------------------
// void* HashNameAllocatorInterface::allocate(const uint64_t p_SizeInBytes)
//{
//  return Internal::g_ResourceAllocator.allocate(p_SizeInBytes, 0);
//}
////-----------------------------------------------------------------------------
// void* HashNameAllocatorInterface::allocateAligned(const uint64_t
// p_SizeInBytes,
//                                                  const uint32_t p_Allignment,
//                                                  const uint32_t p_Offset)
//{
//  return Internal::g_ResourceAllocator.allocateAligned(
//      p_SizeInBytes, p_Allignment, p_Offset, 0);
//} // namespace Foundation
//-----------------------------------------------------------------------------
// HashName
//-----------------------------------------------------------------------------
HashName::HashName() : m_Hash(0u)
{
  callOnce([&]() {
    std::string empty("empty");
    ms_StringRegistery.set(0u, empty);
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
  ms_StringRegistery.set(m_Hash, p_String);
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
  defaultString = ms_StringRegistery.get(m_Hash, defaultString);
  return defaultString.c_str();
}
//-----------------------------------------------------------------------------
Foundation::Container::SimpleFixedMap<
    Foundation::Container::DefaultContainerAllocatorInterface, std::string,
    HashNameMapCapacity>
    HashName::ms_StringRegistery;
//-----------------------------------------------------------------------------
}; // namespace Foundation
