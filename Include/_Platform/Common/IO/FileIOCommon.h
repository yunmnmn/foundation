#include <EASTL/shared_ptr.h>

#include <Memory/ClassAllocator.h>
#include <IO/FileIOInterface.h>

namespace Foundation
{
namespace IO
{
class FileIOCommon;
using FileIO = FileIOCommon;

class FileIOCommon : public FileIOInterface
{
 public:
   static constexpr size_t PageCount = 12u;
   static constexpr size_t MaxDescriptorSetCountPerPage = 32u;
   CLASS_ALLOCATOR_PAGECOUNT_PAGESIZE(FileIOCommon, PageCount,
                                      static_cast<uint32_t>(sizeof(FileIOCommon) * MaxDescriptorSetCountPerPage));

   static eastl::shared_ptr<FileIOInterface> CreateFileIO(FileIODescriptor&& p_descriptor);

   FileIOCommon() = delete;

   void Open() final;
   void Close() final;

 private:
   FileIOCommon(FileIODescriptor&& p_descriptor);

   Util::HashName m_path;
};
} // namespace IO
} // namespace Foundation
