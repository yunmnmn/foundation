#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <util/Util.h>
#include <Util/MurmurHash3.h>
#include <Util/HashName.h>

#include <Std/string_bootstrap.h>
#include <Std/unordered_map_bootstrap.h>

#include <mutex>

namespace Foundation
{
namespace Util
{
using HashKeyType = uint64_t;

struct HashName
{
   HashName();
   HashName(const Std::string_bootstrap& p_String);
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
   static Std::unordered_map_bootstrap<uint64_t, Std::string_bootstrap>& GetStringRegistery();
   uint64_t m_Hash = 0u;

   const char* m_name = nullptr;

   static std::mutex ms_hashNameMutex;
   static bool ms_initialized;
};
}; // namespace Util
}; // namespace Foundation
