#include "Allocator.h"

#include <mutex>

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
LinearAllocator::LinearAllocator(const uint64_t p_SizeInBytes)
    : m_CapacityInBytes(0u), m_offsetInBytes(0u), m_Data(nullptr)
{
  m_Data = malloc(p_SizeInBytes);
  // TODO: assert

  m_CapacityInBytes = p_SizeInBytes;
}
//-----------------------------------------------------------------------------
LinearAllocator::~LinearAllocator()
{
  if (m_Data)
  {
    ::free(m_Data);
    m_Data = nullptr;
    m_CapacityInBytes = 0u;
    m_offsetInBytes = 0u;
  }
}
//-----------------------------------------------------------------------------
void* LinearAllocator::allocate(const uint64_t p_SizeInBytes,
                                const int32_t p_Flags)
{
  if (m_offsetInBytes + p_SizeInBytes > m_CapacityInBytes)
  {
    // assert here
  }
  void* mem = (void*)((uint8_t*)m_Data + m_offsetInBytes);
  m_offsetInBytes = p_SizeInBytes;
  return mem;
}
//-----------------------------------------------------------------------------
void* LinearAllocator::allocateAligned(const uint64_t p_Bytes,
                                       const uint32_t p_Alignment,
                                       const uint32_t p_Offset,
                                       const int32_t p_Flags)
{
  return nullptr;
}
//-----------------------------------------------------------------------------
void LinearAllocator::free(void* p_Memory, const uint64_t p_SizeInBytes)
{
  // Yeah... Don't do this, unless it's the last entry
}
// struct LinearAllocatorMarker
//{
//  LinerAllocator* m_LinearAllocator;
//  uint64_t m_allocatorMarker = 0u;
//
//  LinearAllocatorMarker(LinerAllocator& p_LinearAllocatorMarker)
//      : m_allocatorMarker(p_LinearAllocatorMarker.m_dataPosition),
//        m_LinearAllocator(&p_LinearAllocatorMarker)
//  {
//  }
//
//  ~LinearAllocatorMarker()
//  {
//    m_LinearAllocator->m_dataPosition = m_allocatorMarker;
//  }
//};
//
//-----------------------------------------------------------------------------
// template <AllocatorType type, size_t bytes, typename Allocator>
// Allocator* EastlAllocator<type, bytes, Allocator>::ms_allocator;
// template <AllocatorType type, size_t bytes, typename Allocator>
// std::once_flag EastlAllocator<type, bytes, Allocator>::m_initializeFlag;
//-----------------------------------------------------------------------------
}; // namespace Memory
}; // namespace Foundation
