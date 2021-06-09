#include <Util/HashName.h>

namespace Foundation
{
namespace IO
{
enum class FileIOFlags : uint32_t
{
   FileIOIn = (1 << 0),
   FileIOOut = (1 << 1),
   FileIOBinary = (1 << 2),
};

struct FileIODescriptor
{
   Util::HashName m_path;
   FileIOFlags m_fileIOFlags = static_cast<FileIOFlags>(0);
};

class FileIOInterface
{
 public:
   virtual ~FileIOInterface(){};

   virtual void Open() = 0;
   virtual void Close() = 0;

   // Read data
   virtual void Read(void* p_data, uint64_t p_sizeInBytes) = 0;

   // Get the file size
   virtual uint64_t GetFileSize() = 0;

 private:
};
} // namespace IO
} // namespace Foundation
