#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <experimental/filesystem>
#include <condition_variable>
#include <fstream> // std::ifstream

#include <Util/Util.h>
#include <Container/SimpleLockFreeQueue.h>
#include <Util/Assert.h>
#include <FiberScheduler.h>
#include <RendererIO.h>

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
  using FileSystemQueue =
      Container::MultipleProducerLockFreeQueue<FileRequestDecl*, 128u>;
  //-----------------------------------------------------------------------------
  static void init()
  {
  }
  //-----------------------------------------------------------------------------
  static void destroy()
  {
  }
  //-----------------------------------------------------------------------------
  static bool empty()
  {
    return ms_ProcessQueue.isEmpty();
  }
  //-----------------------------------------------------------------------------
  static void enqueueLoadRequest(FileRequestDecl** p_FiberRequestDecl,
                                 uint32_t p_Count)
  {
    for (uint32_t i = 0u; i < p_Count; i++)
    {
      ms_ProcessQueue.push(p_FiberRequestDecl[i]);
    }

    Renderer::RendererIO::signal();
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
        if (!ms_ProcessQueue.isEmpty())
        {
          processLoadRequests(true);
          lock.release();
        }
        else
        {
          // TODO: put this back
          // ms_ProcessConditionVariable.wait(lock);
        }
      }
    }
  }
  //-----------------------------------------------------------------------------
  static void processLoadRequests(const bool p_AdoptLock)
  {
    // Copy the main queue to a process queue, so the loading part is out of the
    // critical section
    {
      // while (ms_ProcessQueue.pop(fileRequestDecl))
      FileRequestDecl* fileRequestDecl = nullptr;
      while (!ms_ProcessQueue.isEmpty())
      {
        ms_ProcessQueue.pop(fileRequestDecl);
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
  static Foundation::Mutex ms_QueueMutex;

  // TODO: find a better place for this
  const uint64_t RingBufferSize = 1024u * 1024u * 50u;

  static std::mutex ms_ProcessMutex;
};
//-----------------------------------------------------------------------------
template <typename t_Allocator>
typename FileSystem<t_Allocator>::FileSystemQueue
    FileSystem<t_Allocator>::ms_ProcessQueue;
template <typename t_Allocator>
Foundation::Mutex FileSystem<t_Allocator>::ms_QueueMutex;
template <typename t_Allocator>
std::mutex FileSystem<t_Allocator>::ms_ProcessMutex;
//-----------------------------------------------------------------------------
}; // namespace FiberSystem
}; // namespace Foundation
