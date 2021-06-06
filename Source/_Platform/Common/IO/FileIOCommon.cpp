#include <IO/FileIOCommon.h>

#include <Memory/ClassAllocator.h>

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

   // Friend the static factory function to allow access to the constructor
   friend eastl::shared_ptr<FileIOInterface> FileIOCommon::CreateFileIO(FileIODescriptor&& p_descriptor);

 private:
   FileIOCommonImpl(FileIODescriptor&& p_descriptor);

   Util::HashName m_path;
};

eastl::shared_ptr<FileIOInterface> FileIOCommon::CreateFileIO(FileIODescriptor&& p_descriptor)
{
   return eastl::shared_ptr<FileIOInterface>(new FileIOCommonImpl(eastl::move(p_descriptor)));
}

FileIOCommonImpl::FileIOCommonImpl(FileIODescriptor&& p_descriptor)
{
   m_path = p_descriptor.m_path;
}

Foundation::IO::FileIOCommonImpl::~FileIOCommonImpl()
{
}

void FileIOCommonImpl::Open()
{
}
void FileIOCommonImpl::Close()
{
}

} // namespace IO
} // namespace Foundation
