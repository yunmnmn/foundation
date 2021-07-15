#include <IO/FileWatcher.h>

#include <regex>
#include <Windows.h>

#include <EASTL/array.h>
#include <EASTL/utility.h>

#include <Memory/ClassAllocator.h>

#include <IO/FileWatcherInterface.h>

namespace Foundation
{
namespace IO
{

using Regex = std::basic_regex<char>;
class FileWatcherWinImpl : public FileWatcherInterface
{
   // Friend the static factory function to allow access to the constructor
   friend eastl::shared_ptr<FileWatcherInterface> FileWatcherWin::CreateFileWatcher(FileWatcherDescriptor&& p_descriptor);

   using FileEventDescriptor = eastl::pair<Util::HashName, FileWatchEvent>;

 public:
   static constexpr size_t PageCount = 12u;
   static constexpr size_t MaxDescriptorSetCountPerPage = 32u;
   CLASS_ALLOCATOR_PAGECOUNT_PAGESIZE(FileWatcherWinImpl, PageCount,
                                      static_cast<uint32_t>(sizeof(FileWatcherWinImpl) * MaxDescriptorSetCountPerPage));

   FileWatcherWinImpl() = delete;
   ~FileWatcherWinImpl() final;

   // FileWatcherInterface overrides
   void Watch() final;
   void Unwatch() final;

 private:
   FileWatcherWinImpl(FileWatcherDescriptor&& p_descriptor);

   bool PassFilter(const Util::HashName& p_fileName) const;
   HANDLE GetDirectory(const Util::HashName& p_directory);

   void InvokeCallback();
   void WatchDirectory();

   Util::HashName ConvertWideCharToHashName(Std::wstring_bootstrap p_wstring) const;

 private:
   Util::HashName m_directory;
   Util::HashName m_regexPattern;
   std::function<void(const Util::HashName&, FileWatchEvent)> m_callback;

   std::mutex m_callbackMutex;
   std::condition_variable m_conditionalVariable;

   Std::vector_bootstrap<FileEventDescriptor> m_fileEvents;

   // Regular expression to filter only relevant files
   Regex m_regex;

   // Directory handle
   HANDLE m_directoryHandle = {};

   // Used to signal an closing event
   HANDLE m_closeEvent = {};

   // Manages the directory for changes
   std::thread m_watchThread;
   // Calls the callback with the appropriate FilEvent
   std::thread m_callbackInvokerThread;

   // When set to through, the thread to WatchDirectory and InvokeCallback thread will exit
   bool m_destroy = false;
};

eastl::shared_ptr<FileWatcherInterface> FileWatcherWin::CreateFileWatcher(FileWatcherDescriptor&& p_descriptor)
{
   return Std::CreateSharedPtr(new FileWatcherWinImpl(eastl::move(p_descriptor)));
}

FileWatcherWinImpl::FileWatcherWinImpl(FileWatcherDescriptor&& p_descriptor)
{
   m_directory = p_descriptor.m_directory;
   m_regexPattern = p_descriptor.m_regexPattern;
   m_callback = eastl::move(p_descriptor.m_callback);

   m_regex = Regex(m_regexPattern.GetCStr());

   // Create a close event, which will be signaled when the object destroys itself
   m_closeEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
   ASSERT(!m_closeEvent, "Failed to create an Windows Event");

   // Get the directory handle
   m_directoryHandle = GetDirectory(m_directory);
}

FileWatcherWinImpl::~FileWatcherWinImpl()
{
   Unwatch();
   // TODO:
}

void FileWatcherWinImpl::Watch()
{
   m_callbackInvokerThread = std::move(std::thread([this]() { InvokeCallback(); }));

   m_watchThread = std::move(std::thread([this]() { WatchDirectory(); }));
}

void FileWatcherWinImpl::Unwatch()
{
   // Flag the event to close the Watching thread
   m_destroy = true;
   SetEvent(m_closeEvent);
   m_conditionalVariable.notify_all();

   // Wait till the threads are closed
   m_watchThread.join();
   m_callbackInvokerThread.join();

   // Set everything back to the initial state
   m_destroy = false;
   ResetEvent(m_closeEvent);
}

bool FileWatcherWinImpl::PassFilter(const Util::HashName& p_fileName) const
{
   return std::regex_match(p_fileName.GetCStr(), m_regex);
}

HANDLE FileWatcherWinImpl::GetDirectory(const Util::HashName& p_directory)
{
   DWORD file_info = GetFileAttributes(p_directory.GetCStr());
   ASSERT(file_info == INVALID_FILE_ATTRIBUTES, "Failed to get the attributes of the directory");
   ASSERT(file_info == FILE_ATTRIBUTE_DIRECTORY, "Passed path is not a directory");

   HANDLE directory = CreateFile(p_directory.GetCStr(),                                  // pointer to the file name
                                 FILE_LIST_DIRECTORY,                                    // access (read/write) mode
                                 FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // share mode
                                 nullptr,                                                // security descriptor
                                 OPEN_EXISTING,                                          // how to create
                                 FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,      // file attributes
                                 HANDLE(0));                                             // file with attributes to copy
   ASSERT(directory == INVALID_HANDLE_VALUE, "Invalid directory handle");

   return directory;
}

void FileWatcherWinImpl::WatchDirectory()
{
   static const Std::unordered_map_bootstrap<DWORD, FileWatchEvent> NativeEventMap = {
       {FILE_ACTION_ADDED, FileWatchEvent::Added},
       {FILE_ACTION_REMOVED, FileWatchEvent::Removed},
       {FILE_ACTION_MODIFIED, FileWatchEvent::Modified},
       {FILE_ACTION_RENAMED_OLD_NAME, FileWatchEvent::RenamedOld},
       {FILE_ACTION_RENAMED_NEW_NAME, FileWatchEvent::RenamedNew}};

   const uint32_t BufferSize = 1024u * 256u;
   eastl::vector<BYTE> buffer(BufferSize);

   DWORD returnedBytesSize = 0u;
   OVERLAPPED overlappedBuffer = {.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)};

   eastl::array<HANDLE, 2> handles{overlappedBuffer.hEvent, m_closeEvent};

   while (!m_destroy)
   {
      // Read the changes of the watched directory
      const DWORD listenFilters = FILE_NOTIFY_CHANGE_SECURITY | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_ACCESS |
                                  FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_ATTRIBUTES |
                                  FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_FILE_NAME;
      ReadDirectoryChangesW(m_directoryHandle, buffer.data(), static_cast<DWORD>(buffer.size()), TRUE, listenFilters,
                            &returnedBytesSize, &overlappedBuffer, NULL);

      // Add the watched files and its events
      Std::vector_bootstrap<FileEventDescriptor> fileEvents;
      const DWORD result = WaitForMultipleObjects(2, handles.data(), FALSE, INFINITE);
      switch (result)
      {
      case WAIT_OBJECT_0: {
         bool res = GetOverlappedResult(m_directoryHandle, &overlappedBuffer, &returnedBytesSize, TRUE);
         ASSERT(!res, "Failed to GetOverlappedResult");

         if (returnedBytesSize == 0)
         {
            break;
         }

         FILE_NOTIFY_INFORMATION* file_information = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&buffer[0]);
         while (true)
         {
            Std::wstring_bootstrap fileName{file_information->FileName,
                                            file_information->FileNameLength / sizeof(file_information->FileName[0])};

            const Util::HashName changedFileName = ConvertWideCharToHashName(fileName);
            if (PassFilter(changedFileName))
            {
               const auto fileEventIt = NativeEventMap.find(file_information->Action);
               ASSERT(fileEventIt != NativeEventMap.end(), "Native event can't be converted");

               fileEvents.emplace_back(changedFileName, fileEventIt->second);
            }

            // Break out if there is no next event
            if (file_information->NextEntryOffset == 0)
            {
               break;
            }

            file_information = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<BYTE*>(file_information) +
                                                                          file_information->NextEntryOffset);
         }
      }
      break;
      case WAIT_OBJECT_0 + 1:
         // quit
         break;
      case WAIT_FAILED:
         break;
      }

      // Put the FileEvents in the member for the another thread to invoke
      {
         std::lock_guard<std::mutex> lock(m_callbackMutex);
         eastl::swap(m_fileEvents, fileEvents);
      }

      // Wake up the invoker thread
      m_conditionalVariable.notify_all();
   }
};

Util::HashName FileWatcherWinImpl::ConvertWideCharToHashName(Std::wstring_bootstrap p_wstring) const
{
   Std::string_bootstrap multiByteString;
   const int requiredSize =
       WideCharToMultiByte(CP_UTF8, 0, &p_wstring[0], static_cast<uint32_t>(p_wstring.size()), NULL, 0, NULL, NULL);
   multiByteString.resize(requiredSize, '\0');
   WideCharToMultiByte(CP_UTF8, 0, &p_wstring[0], (int)p_wstring.size(), &multiByteString[0], requiredSize, NULL, NULL);

   return eastl::move(Util::HashName(multiByteString));
}

void FileWatcherWinImpl::InvokeCallback()
{
   while (!m_destroy)
   {
      std::unique_lock<std::mutex> lock(m_callbackMutex);
      // Sleep if there are no events, and if the thread needs to be alive
      if (m_fileEvents.empty() && !m_destroy)
      {
         // Only wake up if there are FileEvents to process or if the thread needs to be destroyed
         m_conditionalVariable.wait(lock, [this] { return static_cast<uint32_t>(m_fileEvents.size()) > 0u || m_destroy; });
      }

      Std::vector_bootstrap<FileEventDescriptor> fileEvents;
      eastl::swap(fileEvents, m_fileEvents);

      lock.unlock();

      for (const FileEventDescriptor& fileEvent : fileEvents)
      {
         m_callback(fileEvent.first, fileEvent.second);
      }
   }
}

} // namespace IO

} // namespace Foundation
