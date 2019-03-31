#pragma once

#include <inttypes.h>
#include <stdbool.h>
//#include <atomic>
#include <Windows.h>
#include <filesystem>

#include <util/Assert.h>
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
// TODO: Scratch/Ring buffer to manage data here? Or request a buffer?
// TODO: Add queue for load requests
//-----------------------------------------------------------------------------
struct FileRequestDecl
{
  // TODO: atomic counter to decrement?
  std::atomic<int32_t>* counter;
  const char* filePath;
  void* data;

  uint64_t sizeInBytes;
};
//-----------------------------------------------------------------------------
template <typename t_Allocator> struct FileSystem
{
  using FileSystemQueue =
      Container::SimpleFixedQueue<t_Allocator, FileRequestDecl, 128u>;
  // TODO: Create event
  static void init()
  {
  }

  static void enqueueLoadRequest(FileRequestDecl* p_FiberRequestDecl,
                                 uint32_t p_Count)
  {
    Foundation::ScopedGuard(ms_QueueMutex);
    for (uint32_t i = 0u; i < p_Count; i++)
    {
      ms_Queue.enqueueBack(p_FiberRequestDecl[p_Count]);
    }
  }

  // Processes load requests; if any, else sleep till event gets called
  static void processLoadRequestAndSleep()
  {
    processLoadRequests();

    // TOOD: Let the user handle the loop instead?
    // spinlock for a while first
  }

  static void processLoadRequests()
  {
    {
      Foundation::ScopedGuard(ms_QueueMutex);
      ms_ProcessQueue.clear();
      // Copy queue
      Container::copy(ms_ProcessQueue, ms_Queue);
      ms_Queue.clear();
    }

    FileRequestDecl fberRequestDecl;
    while (ms_ProcessQueue.dequeueFront(&fberRequestDecl))
    {
      // load the file here
    }
  }

private:
  static void _fileExist(){};

  // TODO: hardcoded queue Size
  static FileSystemQueue ms_ProcessQueue;
  static FileSystemQueue ms_Queue;
  static Foundation::Mutex ms_QueueMutex;

  // TODO: find a better place for this
  const uint64_t RingBufferSize = 1024u * 1024u * 10u;
  static Foundation::Memory::LinearAllocator<true> ms_RingBuffer;
};
//-----------------------------------------------------------------------------
}; // namespace FiberSystem
}; // namespace Foundation
