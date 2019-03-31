#include <Memory/Allocator.h>
#include <util/Assert.h>

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
// LinearAllocator
//-----------------------------------------------------------------------------
template <bool t_RingFeature>
LinearAllocator<t_RingFeature>::LinearAllocator(const uint64_t p_SizeInBytes)
    : m_CapacityInBytes(0u), m_offsetInBytes(0u), m_Data(nullptr)
{
  m_Data = malloc(p_SizeInBytes);
  ASSERT(m_Data, "Allocated memory isn't valid");

  m_CapacityInBytes = p_SizeInBytes;
}
//-----------------------------------------------------------------------------
template <bool t_RingFeature> LinearAllocator<t_RingFeature>::~LinearAllocator()
{
  ASSERT(m_Data, "No memory allocated");

  ::free(m_Data);
  m_Data = nullptr;
  m_CapacityInBytes = 0u;
  m_offsetInBytes = 0u;
}
//-----------------------------------------------------------------------------
template <bool t_RingFeature>
void* LinearAllocator<t_RingFeature>::allocate(const uint64_t p_SizeInBytes,
                                               const int32_t p_Flags)
{
  if constexpr (t_RingFeature)
  {
    ASSERT(p_SizeInBytes <= m_CapacityInBytes,
           "Don't call free on a linear allocator");

    if (m_offsetInBytes + p_SizeInBytes > m_CapacityInBytes)
    {
      m_offsetInBytes = 0u;
    }
  }
  else
  {
    ASSERT(m_offsetInBytes + p_SizeInBytes <= m_CapacityInBytes,
           "Don't call free on a linear allocator");
  }

  void* mem = (void*)((uint8_t*)m_Data + m_offsetInBytes);
  m_offsetInBytes = p_SizeInBytes;

  return mem;
}
//-----------------------------------------------------------------------------
template <bool t_RingFeature>
void* LinearAllocator<t_RingFeature>::allocateAligned(
    const uint64_t p_Bytes, const uint32_t p_Alignment, const uint32_t p_Offset,
    const int32_t p_Flags)
{
  return nullptr;
}
//-----------------------------------------------------------------------------
template <bool t_RingFeature>
void LinearAllocator<t_RingFeature>::free(void* p_Memory,
                                          const uint64_t p_SizeInBytes)
{
  ASSERT(false, "Don't call free on a linear allocator");
}
//-----------------------------------------------------------------------------
}; // namespace Memory
}; // namespace Foundation
