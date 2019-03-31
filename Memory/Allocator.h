#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <tlsf/tlsf.h>

namespace Foundation
{
namespace Memory
{
namespace Helper
{
//-----------------------------------------------------------------------------
template <typename t_Allocator> struct AllocatorMarker
{
  AllocatorMarker* m_LinearAllocator;
  uint64_t m_allocatorMarker = 0u;

  AllocatorMarker(t_Allocator& p_LinearAllocatorMarker)
      : m_allocatorMarker(p_LinearAllocatorMarker.m_dataPosition),
        m_LinearAllocator(&p_LinearAllocatorMarker)
  {
  }

  ~AllocatorMarker()
  {
    m_LinearAllocator->m_dataPosition = m_allocatorMarker;
  }
};
//-----------------------------------------------------------------------------
} // namespace Helper
//-----------------------------------------------------------------------------
class TlsfAllocator
{
public:
  TlsfAllocator(const uint64_t p_Bytes);
  ~TlsfAllocator();

  void* allocate(const uint64_t p_SizeInBytes, const int32_t p_Flags);
  void* allocateAligned(const uint64_t p_Bytes, const uint64_t p_Alignment,
                        const uint64_t p_Offset, const uint64_t p_Flags = 0);
  void free(void* p_Memory, const uint64_t p_Size);

private:
  tlsf_t m_Control;
  uint8_t* m_ShadowMemory;
};
//-----------------------------------------------------------------------------
template <bool t_RingFeature = false> class LinearAllocator
{
  friend Helper::AllocatorMarker<LinearAllocator>;

public:
  LinearAllocator(const uint64_t p_SizeInBytes);
  ~LinearAllocator();

  void* allocate(const uint64_t p_SizeInBytes, const int32_t p_Flags);
  void* allocateAligned(const uint64_t p_SizeInBytes,
                        const uint32_t p_Alignment, const uint32_t p_Offset,
                        const int32_t p_Flags);
  void free(void* p_Memory, const uint64_t p_SizeInBytes);

private:
  void* m_Data;
  uint64_t m_CapacityInBytes;
  uint64_t m_offsetInBytes;
};
//-----------------------------------------------------------------------------
}; // namespace Memory
}; // namespace Foundation
//-----------------------------------------------------------------------------
