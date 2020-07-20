#pragma once

#include <inttypes.h>
#include <stdbool.h>

namespace Foundation
{
namespace Memory
{
class SchemaBase
{
 public:
   struct Descriptor
   {
      uint32_t m_maxPageCount = 0u;
      uint32_t m_pageSize = 0u;
   };

   SchemaBase()
   {
   }

 private:
};

}; // namespace Memory
}; // namespace Foundation
