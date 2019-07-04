#pragma once

#include <util/Util.h>
#include <Util/MurmurHash3.h>
#include <Util/HashName.h>

#include <unordered_map>
#include <atomic>
#include <inttypes.h>
#include <stdbool.h>
#include <string>

namespace Foundation
{
//-----------------------------------------------------------------------------
const uint32_t HashNameMapCapacity = 1024u * 10u;
//-----------------------------------------------------------------------------
class SpinLock
{
  std::atomic_flag locked = ATOMIC_FLAG_INIT;

public:
  void lock()
  {
    while (locked.test_and_set(std::memory_order_acquire))
    {
    }
  }
  void unlock()
  {
    locked.clear(std::memory_order_release);
  }
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

  const uint64_t hash() const
  {
    return m_Hash;
  }

private:
  uint64_t m_Hash;

  static std::unordered_map<uint64_t, std::string> ms_StringRegistry;
  static SpinLock ms_SpinLock;
};
}; // namespace Foundation
