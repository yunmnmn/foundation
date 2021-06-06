#include <Util/HashName.h>

namespace Foundation
{
namespace IO
{
struct FileIODescriptor
{
   Util::HashName m_path;
   // TODO: flags
};

class FileIOInterface
{
 public:
   virtual ~FileIOInterface(){};

   virtual void Open() = 0;
   virtual void Close() = 0;

 private:
};
} // namespace IO
} // namespace Foundation
