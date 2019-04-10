#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <experimental/filesystem>
#include <condition_variable>

#include <Util/Util.h>
#include <Container/SimpleFixedQueue.h>

namespace std
{
template <typename t_Type> struct atomic;
}

namespace Foundation
{
namespace FiberSystem
{
//-----------------------------------------------------------------------------
namespace fs = std::experimental::filesystem;
//-----------------------------------------------------------------------------
struct Helper
{
};
//-----------------------------------------------------------------------------
struct FileRequestDecl
{
  uint32_t counterIndex;
  const char* filePath;
  void* buffer;

  uint64_t sizeInBytes;
};
//-----------------------------------------------------------------------------
struct FileSystem
{
  using FileSystemQueue = Container::SimpleFixedQueue<
      Foundation::Container::DefaultContainerAllocatorInterface,
      FileRequestDecl*, 128u>;
  // TODO: Create event
  static void init();
  static void shutdown();
  static void enqueueLoadRequest(FileRequestDecl** p_FiberRequestDecl,
                                 uint32_t p_Count);
  static void signalFileSystem();

  // Processes load requests; if any, else sleep till event gets called
  static void processLoadRequestAndSleep();
  static void processLoadRequests();

private:
  static bool _fileExist(const char* p_Path,
                         fs::file_status p_FileStatus = fs::file_status{});
  static uint32_t _readFile(const char* p_Path, void* p_Buffer);

  // TODO: hardcoded queue Size
  static FileSystemQueue ms_ProcessQueue;
  static FileSystemQueue ms_Queue;
  static Foundation::Mutex ms_QueueMutex;

  // TODO: find a better place for this
  const uint64_t RingBufferSize = 1024u * 1024u * 50u;

  static std::condition_variable ms_ProcessConditionVariable;
  static std::mutex ms_ProcessMutex;
}; // namespace FiberSystem
//-----------------------------------------------------------------------------
}; // namespace FiberSystem
}; // namespace Foundation
