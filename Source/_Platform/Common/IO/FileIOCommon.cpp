#include <IO/FileIOCommon.h>

namespace Foundation
{
namespace IO
{
eastl::shared_ptr<FileIOInterface> FileIOCommon::CreateFileIO(FileIODescriptor&& p_descriptor)
{
   return eastl::shared_ptr<FileIOInterface>(new FileIOCommon(eastl::move(p_descriptor)));
}

FileIOCommon::FileIOCommon(FileIODescriptor&& p_descriptor)
{
   m_path = p_descriptor.m_path;
}

void FileIOCommon::Open()
{
}

void FileIOCommon::Close()
{
}

} // namespace IO
} // namespace Foundation
