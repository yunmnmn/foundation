#include <Std/shared_ptr_bootstrap.h>

#include <IO/FileIOInterface.h>

namespace Foundation
{
namespace IO
{
class FileIOCommon;
using FileIO = FileIOCommon;

class FileIOCommon
{
 public:
   static eastl::shared_ptr<FileIOInterface> CreateFileIO(FileIODescriptor&& p_descriptor);
};
} // namespace IO
} // namespace Foundation
