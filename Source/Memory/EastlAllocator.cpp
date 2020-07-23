#include <Memory/EaStlAllocator.h>

#include <Util/Assert.h>
#include <Util/Util.h>

namespace Foundation
{
namespace Memory
{
EaStlAllocator::EaStlAllocator(const char* p_name)
{
}

EaStlAllocator::EaStlAllocator(const EaStlAllocator& p_other)
{
}

EaStlAllocator::EaStlAllocator(const EaStlAllocator& p_other, const char* p_name)
{
}

void* EaStlAllocator::allocate(size_t p_size, int32_t p_flag = 0)
{
   InitializeSchema();
   return ms_schema->Allocate(p_size);
}

void* EaStlAllocator::allocate(size_t p_size, size_t p_alignment, size_t offset, int flags = 0)
{
   InitializeSchema();
   return ms_schema->AllocateAligned(p_size, p_alignment, offset);
}

void EaStlAllocator::deallocate(void* p, size_t n)
{
   ASSERT(ms_schema.get(), "Bootstrap schema isn't initialized");
   ms_schema->Deallocate(p, n);
}

void EaStlAllocator::InitializeSchema()
{
   CallOnce(ms_initializedFlag, []() {
      // Create the schema descriptor
      BaseSchema::Descriptor desc = {
          .m_maxPageCount = 128u, .m_pageSize = 1024u * 1024u, .m_addPoolCallback = nullptr, .m_removePoolCallback = nullptr};

      // Create the Schema
      ms_schema = TlsfSchema::CreateSchema(desc, (void*)&ms_schemaData);
   });

   ASSERT(ms_schema.get(), "Bootstrap schema isn't initialized");
}

}; // namespace Memory
}; // namespace Foundation
