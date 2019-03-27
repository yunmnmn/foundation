#pragma once

#include <Container/ContainerTypes.h>

namespace Foundation
{
namespace Container
{
//-----------------------------------------------------------------------------
template <typename t_Allocator, typename t_Resource,
          uint32_t t_IncrementStep = 1u>
class SimpleArray
{
  static_assert(!std::is_const_v<t_Resource>,
                "The C++ Standard forbids containers of const elements "
                "because allocator<const T> is ill-formed.");

  using Allocator = t_Allocator;

public:
  SimpleArray() : m_Size(0u), m_Capacity(0u), m_Data(nullptr)
  {
  }

  SimpleArray(const uint32_t p_Size)
      : m_Size(0u), m_Capacity(0u), m_Data(nullptr)
  {
    resize(p_Size);
  }

  SimpleArray(std::initializer_list<t_Resource> p_List)
  {
    SimpleArray();

    resize(p_List.size());
    for (const t_Resource& resource : p_List)
    {
      push_back(resource);
    }
  }

  ~SimpleArray()
  {
    // TODO: Don't call destructors for non-pod types
    for (uint32_t i = 0u; i < m_Size; i++)
    {
      m_Data[i].~t_Resource();
    }
    Allocator::free(m_Data, m_Capacity * sizeof(t_Resource));

    m_Capacity = 0u;
    m_Size = 0u;
  }

  t_Resource* begin()
  {
    return m_Size > 0 ? &m_Data[0] : nullptr;
  }

  const t_Resource* begin() const
  {
    return m_Size > 0 ? &m_Data[0] : nullptr;
  }

  t_Resource* end()
  {
    return m_Size > 0 ? &m_Data[m_Size - 1u] : nullptr;
  }

  const t_Resource* end() const
  {
    return m_Size > 0 ? &m_Data[m_Size - 1u] : nullptr;
  }

  void push_back(t_Resource& p_Entry)
  {
    if (m_Size + 1u >= m_Capacity)
    {
      const uint32_t newSize = m_Capacity + t_IncrementStep;
      grow(newSize);
    }

    Internal::HelperSFINAE::construct<t_Resource>(m_Data + m_Size);
    m_Data[m_Size++] = p_Entry;
  }

  void push_back(const t_Resource& p_Entry)
  {
    if (m_Size + 1u >= m_Capacity)
    {
      const uint32_t newSize = m_Capacity + t_IncrementStep;
      grow(newSize);
    }

    Internal::HelperSFINAE::construct<t_Resource>(m_Data + m_Size);
    m_Data[m_Size++] = p_Entry;
  }

  void pop_back()
  {
    ASSERT(m_Size > 0, "Array is empty");
    erase(m_Size - 1u);
    m_Size--;
  }

  // Grows/Shrinks  the container and create new elements
  void resize(uint32_t p_Size)
  {
    // Grow the container
    if (p_Size > m_Size)
    {
      // Also bigger than the container?
      if (p_Size >= m_Capacity)
      {
        grow(p_Size);
      }
      // Construct new elements
      for (uint32_t i = m_Size; i < p_Size; i++)
      {
        new (m_Data + i) t_Resource();
      }
      return;
    }

    // Shrink the container
    if (p_Size < m_Capacity)
    {
      // Shrink
      shrink(p_Size);
    }
  }
  // Only grows or shrinks the container
  void reserve(uint32_t p_Capacity)
  {
    ASSERT(p_Capacity > m_Capacity,
           "Reserving a smaller capacity than is already available");
    grow(p_Capacity);
  }
  bool empty()
  {
    return m_Size == 0u;
  }

  t_Resource& back()
  {
    ASSERT(m_Size != 0u, "Array is empty");
    return m_Data[m_Size - 1u];
  }
  const t_Resource& back() const
  {
    ASSERT(m_Size != 0u, "Array is empty");
    return m_Data[m_Size - 1u];
  }

  t_Resource& operator[](uint32_t p_Index)
  {
    return m_Data[p_Index];
  }
  const t_Resource& operator[](uint32_t p_Index) const
  {
    return m_Data[p_Index];
  }

  void clear()
  {
    for (uint32_t i = 0u; i < m_Size; i++)
    {
      m_Data[i].~t_Resource();
    }
    m_Size = 0u;
  }

  t_Resource* data()
  {
    return m_Data;
  }

  const uint32_t size() const
  {
    return m_Size;
  }

private:
  //-----------------------------------------------------------------------------
  void erase(const uint32_t p_Index)
  {
    ASSERT(p_Index >= m_Size,
           "Trying to erase entry higher than size, index out of bounds");
    m_Data[p_Index].~t_Resource();
  }
  //-----------------------------------------------------------------------------
  // Only grows the container, without modifying m_Size
  void grow(const uint32_t p_NewCapacity)
  {
    const uint64_t totalSize = p_NewCapacity * sizeof(t_Resource);

    t_Resource* mem = (t_Resource*)Allocator::allocate(totalSize);
    ASSERT(mem, "Allocated memory isn't valid");

    // Note: So, apparently this isn't the same in every standard, so iterate
    // through all objects
    for (uint32_t i = 0u; i < m_Size; i++)
    {
      new (mem + i) t_Resource();
    }

    if (m_Data && m_Capacity && m_Size)
    {
      // Copy old data
      // std::copy(m_Data, m_Data + m_Size, mem);
      Internal::HelperSFINAE::copy(m_Data, m_Data + m_Size, mem);

      // Destruct old data
      for (uint32_t i = 0u; i < m_Size; i++)
      {
        m_Data[i].~t_Resource();
      }
      Allocator::free(m_Data, m_Capacity * sizeof(t_Resource));
    }

    m_Capacity = p_NewCapacity;
    m_Data = (t_Resource*)mem;
  }
  //-----------------------------------------------------------------------------
  // Re-allocates memory, constructs new elements, copies, destructs. Does
  // modify m_Size if the new capacity is smaller than current m_Size
  void shrink(const uint32_t p_NewCapacity)
  {
    const uint64_t totalSize = p_NewCapacity * sizeof(t_Resource);

    t_Resource* mem = (t_Resource*)Allocator::allocate(totalSize);
    ASSERT(mem, "Allocated memory isn't valid");

    // Note: So, apparently this isn't the same in every standard, so iterate
    // through all objects
    const uint32_t minSize = (m_Size < m_Capacity ? m_Size : m_Capacity);
    for (uint32_t i = 0u; i < minSize; i++)
    {
      new (mem + i) t_Resource();
    }

    if (m_Data && m_Capacity)
    {
      // Copy old data
      Internal::HelperSFINAE::copy<t_Resource>(m_Data, m_Data + minSize, mem);
      // std::copy(m_Data, m_Data + minSize, mem);

      // Delete old data
      for (uint32_t i = 0u; i < m_Size; i++)
      {
        m_Data[i].~t_Resource();
      }
      Allocator::free(m_Data, m_Capacity * sizeof(t_Resource));
    }

    m_Capacity = p_NewCapacity;
    m_Size = minSize;
    m_Data = (t_Resource*)mem;
  }
  //-----------------------------------------------------------------------------
private:
  uint32_t m_Size;
  uint32_t m_Capacity;
  t_Resource* m_Data;
};
//-----------------------------------------------------------------------------
template <typename t_Type> struct Entry
{
  uint64_t key;
  t_Type value;
  uint32_t next;
};
//-----------------------------------------------------------------------------
const uint32_t MapEnd = (uint32_t)-1;
struct FindResult
{
  FindResult() : hashIndex(MapEnd), dataPrevIndex(MapEnd), dataIndex(MapEnd)
  {
  }
  uint32_t hashIndex;
  uint32_t dataPrevIndex;
  uint32_t dataIndex;
};
//-----------------------------------------------------------------------------
}; // namespace Container
}; // namespace Foundation
