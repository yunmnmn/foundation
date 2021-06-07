#include <IO/FileIOCommon.h>

#include <fstream>

#include <Memory/ClassAllocator.h>
#include <Std/unordered_map_bootstrap.h>

namespace Foundation
{
namespace IO
{

class FileIOCommonImpl : public FileIOInterface
{
 public:
   static constexpr size_t PageCount = 12u;
   static constexpr size_t MaxDescriptorSetCountPerPage = 32u;
   CLASS_ALLOCATOR_PAGECOUNT_PAGESIZE(FileIOCommonImpl, PageCount,
                                      static_cast<uint32_t>(sizeof(FileIOCommonImpl) * MaxDescriptorSetCountPerPage));

   FileIOCommonImpl() = delete;
   ~FileIOCommonImpl() final;

   void Open() final;
   void Close() final;

   void Read(void* p_data, uint64_t p_sizeInBytes) final;
   uint64_t GetFileSize() final;

   // Friend the static factory function to allow access to the constructor
   friend eastl::shared_ptr<FileIOInterface> FileIOCommon::CreateFileIO(FileIODescriptor&& p_descriptor);

 private:
   FileIOCommonImpl(FileIODescriptor&& p_descriptor);

   Util::HashName m_path;
   FileIOFlags m_fileIOFlags = static_cast<FileIOFlags>(0);

   std::fstream m_fileStream;
};

eastl::shared_ptr<FileIOInterface> FileIOCommon::CreateFileIO(FileIODescriptor&& p_descriptor)
{
   return eastl::shared_ptr<FileIOInterface>(new FileIOCommonImpl(eastl::move(p_descriptor)));
}

FileIOCommonImpl::FileIOCommonImpl(FileIODescriptor&& p_descriptor)
{
   m_path = p_descriptor.m_path;
   m_fileIOFlags = p_descriptor.m_fileIOFlags;
}

Foundation::IO::FileIOCommonImpl::~FileIOCommonImpl()
{
   if (m_fileStream.is_open())
   {
      m_fileStream.close();
   }
}

void FileIOCommonImpl::Open()
{
   static const Foundation::Std::unordered_map_bootstrap<FileIOFlags, std::ios_base::openmode> FileIOFlagsToNativeMap = {
       {FileIOFlags::FileIOIn, std::fstream::in},
       {FileIOFlags::FileIOOut, std::fstream::out},
       {FileIOFlags::FileIOBinar, std::fstream::binary},
   };

   // TODO: Statically check if it's all 32 bit flags
   // Calculate the FileIOFlags to native
   uint32_t bits = 0u;
   {
      for (uint32_t i = 0u; i < 32u; i++)
      {
         const uint32_t currentBit = (1 << i);
         if (currentBit & static_cast<uint32_t>(m_fileIOFlags))
         {
            const auto& mapIt = FileIOFlagsToNativeMap.find(static_cast<FileIOFlags>(currentBit));
            ASSERT(mapIt != FileIOFlagsToNativeMap.end(), "FileIOFlags conversion to std::ios_base::openmode doesn't exist");

            bits |= static_cast<uint32_t>(mapIt->second);
         }
      }
   }
   const std::ios_base::openmode openMode = static_cast<std::ios_base::openmode>(bits);

   m_fileStream.open(m_path.GetCStr(), openMode);
}

void FileIOCommonImpl::Close()
{
   m_fileStream.close();
}

void FileIOCommonImpl::Read(void* p_data, uint64_t p_sizeInBytes)
{
   // Cache the current position in the stream
   std::streampos cachedStreamPosition = m_fileStream.tellg();

   m_fileStream.seekg(0, m_fileStream.beg);
   m_fileStream.read(static_cast<char*>(p_data), p_sizeInBytes);

   // Set the stream back to the cached position
   m_fileStream.seekg(cachedStreamPosition);
}

uint64_t FileIOCommonImpl::GetFileSize()
{
   // Cache the current position in the stream
   std::streampos cachedStreamPosition = m_fileStream.tellg();

   // Set the stream to the end, and get the position
   m_fileStream.seekg(0, m_fileStream.end);
   const uint64_t fileLenght = m_fileStream.tellg();

   // Set the stream back to the cached position
   m_fileStream.seekg(cachedStreamPosition);

   return fileLenght;
}

} // namespace IO
} // namespace Foundation
