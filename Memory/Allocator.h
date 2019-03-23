#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <tlsf/tlsf.h>

namespace Foundation
{
namespace Memory
{
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
class LinearAllocator
{
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
/*template <AllocatorType t_Type, size_t t_Bytes,
          typename t_Allocator = RendererAllocator>
class EastlAllocator
{
public:
  EastlAllocator(const char* p_Name = EASTL_ALLOCATOR_DEFAULT_NAME)
  {
    m_Name = p_Name;
  }
  EastlAllocator(const EastlAllocator& p_Allocator)
  {
    m_Name = p_Allocator.m_Name;
  }
  EastlAllocator(const EastlAllocator& x, const char* p_Name)
  {
    m_Name = p_Name;
  }
  EastlAllocator& operator=(const EastlAllocator& p_Allocator)
  {
    m_Name = p_Allocator.m_Name;
  }

  void* allocate(size_t p_Bytes, int32_t p_Flags = 0)
  {
    std::call_once(m_initializeFlag,
                   [&]() { ms_allocator = new Allocator(bytes); });

    return ms_allocator->allocate(p_Bytes, p_Flags);
  }

  void* allocate(size_t p_Bytes, size_t p_Alignment, size_t p_Offset,
                 int32_t p_Flags = 0)
  {
    std::call_once(m_initializeFlag,
                   [&]() { ms_allocator = new Allocator(bytes); });

    return ms_allocator->allocateAligned(p_Bytes, p_Alignment, p_Offset,
                                         p_Flags);
  }

  void deallocate(void* p_Memory, size_t p_Size)
  {
    ms_allocator->deallocate(p_Memory, p_Size);
  }

private:
  static Allocator* ms_allocator;
  static std::once_flag m_initializeFlag;

  AllocatorType m_type = type;
};
*/
//-----------------------------------------------------------------------------
}; // namespace Memory
}; // namespace Foundation
//-----------------------------------------------------------------------------
