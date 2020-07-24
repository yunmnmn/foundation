#include <Memory/EaStlAllocator.h>

#include <Util/Assert.h>
#include <Util/Util.h>

#include <Memory/MemoryManagerInterface.h>
#include <Memory/BaseSchema.h>
#include <Memory/BootstrapAllocator.h>

namespace Foundation
{
namespace Memory
{
//
// void* EaStlAllocator::allocate(size_t p_size, int32_t p_flag)
//{
//   return Allocate(p_size);
//}
//
// void* EaStlAllocator::allocate(size_t p_size, size_t p_alignment, size_t offset, int flags)
//{
//   return AllocateAllign(p_size, p_alignment);
//}
//
// void EaStlAllocator::deallocate(void* p_address, size_t p_size)
//{
//   Deallocate(p_address, p_size);
//}
//
// void* EaStlAllocator::AllocateInternal(uint64_t p_size)
//{
//   return m_schema->Allocate(p_size);
//}
//
// void* EaStlAllocator::AllocateAlignInternal(uint64_t p_size, uint32_t p_alignment)
//{
//   // TODO FIx the parameters
//   return m_schema->AllocateAligned(p_size, p_alignment, 0);
//}
//
// void EaStlAllocator::DeallocateInternal(void* p_pointer, uint64_t)
//{
//   // TODO FIx the parameters
//   m_schema->Deallocate(p_pointer, 0);
//}

}; // namespace Memory
}; // namespace Foundation
