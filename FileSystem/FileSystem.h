#pragma once

namespace Foundation
{
namespace FileSystem
{
//-----------------------------------------------------------------------------
// TODO: Scratch/Ring buffer to manage data here? Or request a buffer?
//-----------------------------------------------------------------------------
struct LoadRequestDecl
{
  // TODO: atomic counter to decrement
  const char* filePath;
  void* data;
};
// TOOD: Make this File system fiber specific?
//-----------------------------------------------------------------------------
struct FileSystem
{
  // TODO: Create event
  static void init()
  {
  }

  static void enqueueLoadRequest()
  {
  }

  // Processes load requests if any, else sleep till event gets called
  static void processLoadRequestAndSleep()
  {
    while (true)
    {
      // TOOD: Check for load requests
      // WaitForSingleObject()
    }
  }

private:
  static void _dequeueLoadRequest()
  {
  }
};
//-----------------------------------------------------------------------------
}; // namespace FileSystem
}; // namespace Foundation
