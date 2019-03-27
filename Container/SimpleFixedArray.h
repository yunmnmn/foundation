#pragma once

#include <Container/ContainerTypes.h>

namespace Foundation
{
namespace Container
{
//-----------------------------------------------------------------------------
template <typename t_Allocator, typename t_Resource, uint32_t t_Capacity>
class SimpleFixedArray
{
  static_assert(!std::is_const_v<t_Resource>,
                "The C++ Standard forbids containers of const elements "
                "because allocator<const T> is ill-formed.");

public:
  SimpleFixedArray() : m_Size(0u), m_Capacity(t_Capacity), m_Data(nullptr)
  {
    uint64_t allocatedSize = t_Capacity * sizeof(t_Resource);
    m_Data = (t_Resource*)t_Allocator::allocate(allocatedSize);

    ASSERT(m_Data, "Memory allocation failed");

    m_Capacity = t_Capacity;
    m_Size = 0u;
  }

  SimpleFixedArray(std::initializer_list<t_Resource> p_List)
  {
    SimpleFixedArray();

    resize(p_List.size());
    for (const t_Resource& resource : p_List)
    {
      push_back(resource);
    }
  }

  ~SimpleFixedArray()
  {
    // Call all the destructors
    clear();
    m_Capacity = 0u;
  }

  void push_back(const t_Resource& p_Entry)
  {
    ASSERT(m_Size <= m_Capacity, "Fixed Array is full");

    new (m_Data + m_Size) t_Resource();
    m_Data[m_Size++] = p_Entry;
  }

  void resize(uint32_t p_Size)
  {
    ASSERT(p_Size > m_Size, "Resizing failed, given size is too large");
    m_Size = p_Size;
  }

  bool empty()
  {
    return m_Size == 0u;
  }

  const uint32_t size() const
  {
    return m_Size;
  }

  t_Resource& operator[](uint32_t p_Index)
  {
    ASSERT(m_Size > 0u, "No elements in the array");
    ASSERT(p_Index <= m_Size - 1u, "Resizing failed, given size is too large");
    return m_Data[p_Index];
  }

  t_Resource& back()
  {
    // TODO: insert return statement here
    return m_Data[m_Size - 1u];
  }

  const t_Resource& back() const
  {
    // TODO: insert return statement here
    return m_Data[m_Size - 1u];
  }

  void clear()
  {
    for (uint32_t i = 0u; i < m_Size; i++)
    {
      m_Data[i].~t_Resource();
    }
    m_Size = 0u;
  }

  uint32_t m_Size;
  uint32_t m_Capacity;
  t_Resource* m_Data;
};
//-----------------------------------------------------------------------------
}; // namespace Container
}; // namespace Foundation
