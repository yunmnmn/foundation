#pragma once

#include <Util/HashName.h>
#include <functional>

namespace Foundation
{
namespace IO
{
enum class FileWatchEvent : uint32_t
{
   Added = 0u,
   Removed,
   Modified,
   RenamedOld,
   RenamedNew,

   Count,
   Invalid = Count
};

struct FileWatcherDescriptor
{
   Util::HashName m_directory;
   Util::HashName m_regexPattern;

   std::function<void(const Util::HashName&, FileWatchEvent)> m_callback;
};

class FileWatcherInterface
{
 public:
   virtual ~FileWatcherInterface(){};

   virtual void Watch() = 0;
   virtual void Unwatch() = 0;
};
} // namespace IO
} // namespace Foundation
