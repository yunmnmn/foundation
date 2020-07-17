#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <util/Assert.h>
#include <tlsf.h>

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
template <bool t_RingFeature> class LinearAllocator
{
  friend Helper::AllocatorMarker<LinearAllocator>;

public:
  LinearAllocator(const uint64_t p_SizeInBytes)
  {
    m_Data = malloc(p_SizeInBytes);
    ASSERT(m_Data, "Allocated memory isn't valid");

    m_CapacityInBytes = p_SizeInBytes;
  }

  ~LinearAllocator()
  {
    ASSERT(m_Data, "No memory allocated");

    ::free(m_Data);
    m_Data = nullptr;
    m_CapacityInBytes = 0u;
    m_offsetInBytes = 0u;
  }

  void* allocate(const uint64_t p_SizeInBytes, const int32_t p_Flags)
  {
    if constexpr (t_RingFeature)
    {
      if (m_offsetInBytes + p_SizeInBytes > m_CapacityInBytes)
      {
        m_offsetInBytes = 0u;
      }
    }
    else
    {
      ASSERT(m_offsetInBytes + p_SizeInBytes <= m_CapacityInBytes,
             "Not enough memory");
    }

    void* mem = (void*)((uint8_t*)m_Data + m_offsetInBytes);
    m_offsetInBytes = p_SizeInBytes;

    return mem;
  }

  void* allocateAligned(const uint64_t p_SizeInBytes,
                        const uint32_t p_Alignment, const uint32_t p_Offset,
                        const int32_t p_Flags)
  {
    // TODO: implement this
    return nullptr;
  }

  void free(void* p_Memory, const uint64_t p_SizeInBytes)
  {
    ASSERT(false, "Don't call free on a linear allocator");
  }

private:
  void* m_Data;
  uint64_t m_CapacityInBytes;
  uint64_t m_offsetInBytes;
};
//-----------------------------------------------------------------------------
}; // namespace Memory
}; // namespace Foundation
//-----------------------------------------------------------------------------
