#include <Memory/Allocator.h>

#include <mutex> // For free, malloc etc

namespace Foundation
{
namespace Memory
{
//-----------------------------------------------------------------------------
// TlsfAllocator
//-----------------------------------------------------------------------------
TlsfAllocator::TlsfAllocator(const uint64_t p_Bytes)
{
  const uint64_t totalSize = p_Bytes + tlsf_size();

  m_ShadowMemory = static_cast<uint8_t*>(malloc(totalSize));
  m_Control = tlsf_create_with_pool(m_ShadowMemory, totalSize);
}
//-----------------------------------------------------------------------------
TlsfAllocator::~TlsfAllocator()
{
  // Todo: deallocate
}
//-----------------------------------------------------------------------------
void* TlsfAllocator::allocate(const uint64_t p_Bytes, const int32_t p_Flags)
{
  return tlsf_malloc(m_Control, p_Bytes);
}
//-----------------------------------------------------------------------------
void* TlsfAllocator::allocateAligned(const uint64_t p_Bytes,
                                     const uint64_t p_Alignment,
                                     const uint64_t p_Offset,
                                     const uint64_t p_Flags)
{
  return tlsf_memalign(m_Control, p_Alignment, p_Bytes);
}
//-----------------------------------------------------------------------------
void TlsfAllocator::free(void* p_Memory, const uint64_t p_Size)
{
  tlsf_free(m_Control, p_Memory);
}
//-----------------------------------------------------------------------------
}; // namespace Memory
}; // namespace Foundation
