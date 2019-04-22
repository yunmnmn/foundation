#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <experimental/filesystem>
#include <condition_variable>
#include <fstream> // std::ifstream

#include <Util/Util.h>
#include <Container/SimpleFixedQueue.h>
#include <util/Assert.h>
#include <FiberScheduler.h>

namespace std
{
template <typename t_Type> struct atomic;
}

using namespace rapidjson;
using namespace std;

namespace fs = std::experimental::filesystem;

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
  // Feed
  uint32_t counterIndex;
  const char* filePath;
  bool addNullTerminator;

  // Result
  uint64_t sizeInBytes;
  void* buffer;
};
//-----------------------------------------------------------------------------
template <typename t_Allocator> struct FileSystem
{
  //-----------------------------------------------------------------------------
  using FileSystemQueue = Container::SimpleFixedQueue<
      Foundation::Container::DefaultContainerAllocatorInterface,
      FileRequestDecl*, 128u>;
  //-----------------------------------------------------------------------------
  // TODO: Create event
  static void init()
  {
  }
  //-----------------------------------------------------------------------------
  static void shutdown()
  {
  }
  //-----------------------------------------------------------------------------
  static void enqueueLoadRequest(FileRequestDecl** p_FiberRequestDecl,
                                 uint32_t p_Count)
  {
    std::unique_lock<std::mutex> lock(ms_ProcessMutex);

    for (uint32_t i = 0u; i < p_Count; i++)
    {
      ms_Queue.enqueueBack(p_FiberRequestDecl[i]);
    }

    ms_ProcessConditionVariable.notify_all();
  }
  //-----------------------------------------------------------------------------
  static void signalFileSystem()
  {
  }
  //-----------------------------------------------------------------------------
  // Processes load requests; if any, else sleep till event gets called
  static void processLoadRequestAndSleep()
  {
    while (true)
    {
      std::unique_lock<std::mutex> lock(ms_ProcessMutex, std::defer_lock);
      if (!lock.try_lock())
      {
        continue;
      }
      else
      {
        if (ms_Queue.size())
        {
          processLoadRequests();
          lock.release();
        }
        else
        {
          ms_ProcessConditionVariable.wait(lock);
        }
      }
    }
  }
  //-----------------------------------------------------------------------------
  static void processLoadRequests()
  {
    // Copy the main queue to a process queue, so the loading part is out of the
    // critical section
    {
      std::unique_lock<std::mutex> lock(ms_ProcessMutex, std::adopt_lock);

      ms_ProcessQueue.clear();
      ms_ProcessQueue = ms_Queue;
      ms_Queue.clear();

      lock.unlock();
    }

    FileRequestDecl* fileRequestDecl;
    while (ms_ProcessQueue.dequeueFront(fileRequestDecl))
    {
      ASSERT(fileRequestDecl, "Loadrequest not valid");
      ASSERT(fileRequestDecl->filePath, "Loadrequest filepath not valid");
      // ASSERT(fiberRequestDecl->buffer, "Loadrequest buffer not valid");

      _readFile(fileRequestDecl);

      // Subtract the counter
      ::Foundation::FiberSystem::AtomicCounter* counter = nullptr;
      FiberSystem::FiberManager::getAtomicCounterFromIndex(
          fileRequestDecl->counterIndex, counter);
      counter->fetch_sub(1u, std::memory_order_relaxed);
    }
  }

private:
  static bool _fileExist(const char* p_Path,
                         fs::file_status p_FileStatus = fs::file_status{})
  {
    if (fs::status_known(p_FileStatus) ? fs::exists(p_FileStatus)
                                       : fs::exists(p_Path))
      return true;
    else
      return false;
  }
  //-----------------------------------------------------------------------------
  static void _readFile(FileRequestDecl* p_FileRequest)
  {
    ASSERT(_fileExist(p_FileRequest->filePath), "File doesn't exist");

    std::ifstream file(p_FileRequest->filePath,
                       std::ios::binary | std::ios::ate);
    uint32_t size = (uint32_t)file.tellg();
    file.seekg(0, std::ios::beg);

    uint32_t allocateSize =
        (p_FileRequest->addNullTerminator) ? size + 1u : size;

    p_FileRequest->buffer = t_Allocator::allocate(allocateSize);
    p_FileRequest->sizeInBytes = allocateSize;

    if (file.read((char*)p_FileRequest->buffer, size))
    {
      /* worked! */
    }
    else
    {
      ASSERT(false, "Failed to load");
    }

    file.close();

    // Add null terminator
    if (p_FileRequest->addNullTerminator)
    {
      *((char*)p_FileRequest->buffer + size) = '\0';
    }
  }
  //-----------------------------------------------------------------------------
  // TODO: hardcoded queue Size
  static FileSystemQueue ms_ProcessQueue;
  static FileSystemQueue ms_Queue;
  static Foundation::Mutex ms_QueueMutex;

  // TODO: find a better place for this
  const uint64_t RingBufferSize = 1024u * 1024u * 50u;

  static std::condition_variable ms_ProcessConditionVariable;
  static std::mutex ms_ProcessMutex;
};
//-----------------------------------------------------------------------------
template <typename t_Allocator>
typename FileSystem<t_Allocator>::FileSystemQueue
    FileSystem<t_Allocator>::ms_ProcessQueue;
template <typename t_Allocator>
typename FileSystem<t_Allocator>::FileSystemQueue
    FileSystem<t_Allocator>::ms_Queue;
template <typename t_Allocator>
Foundation::Mutex FileSystem<t_Allocator>::ms_QueueMutex;
template <typename t_Allocator>
std::condition_variable FileSystem<t_Allocator>::ms_ProcessConditionVariable;
template <typename t_Allocator>
std::mutex FileSystem<t_Allocator>::ms_ProcessMutex;
//-----------------------------------------------------------------------------
}; // namespace FiberSystem
}; // namespace Foundation
