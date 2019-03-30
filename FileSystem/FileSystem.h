#pragma once

#include <inttypes.h>
#include <stdbool.h>
//#include <atomic>
#include <Windows.h>

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
};
// TOOD: Make this File system fiber specific?
//-----------------------------------------------------------------------------
template <typename t_Allocator> struct FileSystem
{
  // TODO: Create event
  static void init()
  {
  }

  static void enqueueLoadRequest()
  {
  }

  // Processes load requests; if any, else sleep till event gets called
  static void processLoadRequestAndSleep(FileRequestDecl* p_FiberRequestDecl,
                                         uint32_t p_Count)
  {
    for (uint32_t i = 0u; i < p_Count; i++)
    {
    }
    // TOOD: Let the user handle the loop instead?
    // spinlock for a while first
  }

  static void processLoadRequests()
  {
  }

private:
  static void _dequeueLoadRequest(){

  };

  // TODO: hardcoded queue Size
  Containers::SimpleFixedQueue<t_Allocator, FileRequestDecl, 1000u> m_Queue;
};
//-----------------------------------------------------------------------------
}; // namespace FiberSystem
}; // namespace Foundation
