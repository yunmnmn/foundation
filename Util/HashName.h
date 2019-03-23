#pragma once

//#include <iostream>

#include <inttypes.h>
#include <stdbool.h>
// TODO: replace string
#include <string>

#include <util/Util.h>
#include <Util/MurmurHash3.h>
#include <Util/HashName.h>
#include <Container/ContainerTypes.h>

namespace Foundation
{
//-----------------------------------------------------------------------------
const uint32_t HashNameMapCapacity = 1024u * 10u;
//-----------------------------------------------------------------------------
class HashNameAllocatorInterface
{
public:
  HashNameAllocatorInterface();

  ~HashNameAllocatorInterface();

  static void* allocate(const uint64_t p_SizeInBytes);
  static void* allocateAligned(const uint64_t p_SizeInBytes,
                               const uint32_t p_Allignment,
                               const uint32_t p_Offset);
};
//-----------------------------------------------------------------------------
struct HashName
{
  HashName();
  HashName(const std::string& p_String);
  HashName(const HashName& p_Rhs);
  ~HashName();

  bool operator==(const HashName& p_Rhs);
  const char* c_str() const;

private:
  uint64_t m_Hash;

  static Foundation::Container::FixedSimpleMap<HashNameAllocatorInterface,
                                               std::string, HashNameMapCapacity>
      ms_StringRegistery;
};
}; // namespace Foundation
