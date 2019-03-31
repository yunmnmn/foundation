#include <FileSystem/FileSystem.h>

#include <Windows.h>
#include <fstream> // std::ifstream

#include <util/Assert.h>
#include <Container/SimpleFixedQueue.h>

namespace fs = std::experimental::filesystem;

namespace Foundation
{
namespace FiberSystem
{
//-----------------------------------------------------------------------------
FileSystem::FileSystemQueue FileSystem::ms_ProcessQueue;
FileSystem::FileSystemQueue FileSystem::ms_Queue;
Foundation::Mutex FileSystem::ms_QueueMutex;
//-----------------------------------------------------------------------------
void FileSystem::init()
{
}
//-----------------------------------------------------------------------------
void FileSystem::enqueueLoadRequest(FileRequestDecl*& p_FiberRequestDecl,
                                    uint32_t p_Count)
{
  Foundation::ScopedGuard(ms_QueueMutex);
  for (uint32_t i = 0u; i < p_Count; i++)
  {
    ms_Queue.enqueueBack(p_FiberRequestDecl[p_Count]);
  }
}
//-----------------------------------------------------------------------------
void FileSystem::processLoadRequestAndSleep()
{
  processLoadRequests();

  // TOOD: Let the user handle the loop instead?
  // spinlock for a while first
}
//-----------------------------------------------------------------------------
void FileSystem::processLoadRequests()
{
  {
    Foundation::ScopedGuard(ms_QueueMutex);
    ms_ProcessQueue.clear();
    // Copy queue
    Container::copy(ms_ProcessQueue, ms_Queue);
    ms_Queue.clear();
  }

  FileRequestDecl* fberRequestDecl;
  while (ms_ProcessQueue.dequeueFront(&fberRequestDecl))
  {
    fberRequestDecl->sizeInBytes =
        _readFile(fberRequestDecl->filePath, fberRequestDecl->buffer);

    // TODO: proper
    // fberRequestDecl.counter--;
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
  // load the file here
  std::ifstream file(p_Path, std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  if (file.read((char*)p_Buffer, size))
  {
    /* worked! */
  }
}
//-----------------------------------------------------------------------------
}; // namespace FiberSystem
}; // namespace Foundation
