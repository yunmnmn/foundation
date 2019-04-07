#include <FileSystem/FileSystem.h>

#include <Windows.h>
#include <fstream> // std::ifstream

#include <util/Assert.h>
#include <Container/SimpleFixedQueue.h>
#include <FiberScheduler.h>

using namespace rapidjson;
using namespace std;

namespace fs = std::experimental::filesystem;

namespace Foundation
{
namespace FiberSystem
{
//-----------------------------------------------------------------------------
FileSystem::FileSystemQueue FileSystem::ms_ProcessQueue;
FileSystem::FileSystemQueue FileSystem::ms_Queue;
Foundation::Mutex FileSystem::ms_QueueMutex;
std::condition_variable FileSystem::ms_ProcessConditionVariable;
std::mutex FileSystem::ms_ProcessMutex;
//-----------------------------------------------------------------------------
void FileSystem::init()
{
}
//-----------------------------------------------------------------------------
void FileSystem::shutdown()
{
}
//-----------------------------------------------------------------------------
void FileSystem::enqueueLoadRequest(FileRequestDecl** p_FiberRequestDecl,
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
void FileSystem::signalFileSystem()
{
}
//-----------------------------------------------------------------------------
void FileSystem::processLoadRequestAndSleep()
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
void FileSystem::processLoadRequests()
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

  FileRequestDecl* fiberRequestDecl;
  while (ms_ProcessQueue.dequeueFront(fiberRequestDecl))
  {
    ASSERT(fiberRequestDecl, "Loadrequest not valid");
    ASSERT(fiberRequestDecl->filePath, "Loadrequest filepath not valid");
    ASSERT(fiberRequestDecl->buffer, "Loadrequest buffer not valid");

    fiberRequestDecl->sizeInBytes =
        _readFile(fiberRequestDecl->filePath, fiberRequestDecl->buffer);

    // Subtract the counter
    AtomicCounter* counter = nullptr;
    FiberSystem::FiberManager::getAtomicCounterFromIndex(
        fiberRequestDecl->counterIndex, counter);
    counter->fetch_sub(1u, std::memory_order_relaxed);
  }
}
//-----------------------------------------------------------------------------
bool FileSystem::_fileExist(const char* p_Path, fs::file_status p_FileStatus)
{
  if (fs::status_known(p_FileStatus) ? fs::exists(p_FileStatus)
                                     : fs::exists(p_Path))
    return true;
  else
    return false;
}
//-----------------------------------------------------------------------------
uint32_t FileSystem::_readFile(const char* p_Path, void* p_Buffer)
{
  // using CustonDocument = GenericDocument<UTF8<>>;

  // load the file here
  std::ifstream file(p_Path, std::ios::binary | std::ios::ate);
  uint32_t size = (uint32_t)file.tellg();
  file.seekg(0, std::ios::beg);

  ASSERT(_fileExist(p_Path), "File doesn't exist");

  if (file.read((char*)p_Buffer, size))
  {
    /* worked! */
  }
  else
  {
    ASSERT(false, "Failed to load");
  }

  file.close();

  return size;
}
//-----------------------------------------------------------------------------

// class CrtAllocator
//{
// public:
//  static const bool kNeedFree = true;
//  void* Malloc(size_t size)
//  {
//    if (size) //  behavior of malloc(0) is implementation defined.
//      return std::malloc(size);
//    else
//      return NULL; // standardize to returning NULL.
//  }
//  void* Realloc(void* originalPtr, size_t originalSize, size_t newSize)
//  {
//    (void)originalSize;
//    if (newSize == 0)
//    {
//      std::free(originalPtr);
//      return NULL;
//    }
//    return std::realloc(originalPtr, newSize);
//  }
//  static void Free(void* ptr)
//  {
//    std::free(ptr);
//  }
//};
}; // namespace FiberSystem
}; // namespace Foundation
