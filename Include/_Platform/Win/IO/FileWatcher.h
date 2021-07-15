#pragma once

#include <Std/shared_ptr_bootstrap.h>

#include <IO/FileWatcherInterface.h>

namespace Foundation
{
namespace IO
{
class FileWatcherWin
{
 public:
   static eastl::shared_ptr<FileWatcherInterface> CreateFileWatcher(FileWatcherDescriptor&& p_descriptor);
};

using FileWatcher = FileWatcherWin;

} // namespace IO
} // namespace Foundation
