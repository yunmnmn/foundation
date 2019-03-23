#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <cstring>
#include <algorithm>

#include <Util/Assert.h>

namespace Foundation
{
namespace Container
{
namespace Internal
{
//-----------------------------------------------------------------------------
struct HelperSFINAE
{
  template <typename t_Type>
  static void copy(t_Type* const p_Begin, t_Type* const p_End,
                   t_Type* const p_Dest)
  {
    if constexpr (std::is_pod_v<t_Type>)
    {
      const uint64_t test = p_End - p_Begin;
      memcpy((void*)p_Dest, (void*)p_Begin, test);
    }
    else
    {
      t_Type* mem = p_Begin;
      while (mem != p_End)
      {
        *p_Dest = *mem;
        mem++;
      }
    }
  }

  // template <typename t_Type>
  // constexpr std::enable_if<std::is_constructible_v<t_Type>, void>
  // construct(t_Type* p_Memory)
  //{
  //  // new (p_Memory) t_Resource();
  //}

  // template <typename t_Type>
  // constexpr std::enable_if_t<!std::is_constructible_v<t_Type>, void>
  // construct(t_Type* p_Memory)
  //{
  //  // new (p_Memory) t_Resource();
  //}

  template <typename t_Type> static void construct(t_Type* p_Memory)
  {
    if constexpr (std::is_constructible_v<t_Type>)
      ::new (p_Memory) t_Type;
    else
    {
    }
  }
}; // namespace Internal
//-----------------------------------------------------------------------------
}; // namespace Internal
//-----------------------------------------------------------------------------
// TODO: add option for POD and non-POD containers
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
  // Only grows the container, without modifying the m_Size
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
}; // namespace Container
//-----------------------------------------------------------------------------
const uint32_t MapEnd = (uint32_t)-1;
//-----------------------------------------------------------------------------
template <typename t_Type> struct Entry
{
  uint64_t key;
  t_Type value;
  uint32_t next;
};
//-----------------------------------------------------------------------------
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
template <typename t_Allocator, typename t_Resource, uint32_t t_Capacity>
class FixedSimpleArray
{
  static_assert(!std::is_const_v<t_Resource>,
                "The C++ Standard forbids containers of const elements "
                "because allocator<const T> is ill-formed.");

public:
  FixedSimpleArray() : m_Size(0u), m_Capacity(t_Capacity), m_Data(nullptr)
  {
    uint64_t allocatedSize = t_Capacity * sizeof(t_Resource);
    m_Data = (t_Resource*)t_Allocator::allocate(allocatedSize);

    ASSERT(m_Data, "Memory allocation failed");

    m_Capacity = t_Capacity;
    m_Size = 0u;
  }

  FixedSimpleArray(std::initializer_list<t_Resource> p_List)
  {
    FixedSimpleArray();

    resize(p_List.size());
    for (const t_Resource& resource : p_List)
    {
      push_back(resource);
    }
  }

  ~FixedSimpleArray()
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
template <typename t_Type> class SimpleMap
{
  // Array m_Data;
  // Array m_Hash;

  // SimpleArray m_Hash;
  // SimpleArray m_Data;
};
//-----------------------------------------------------------------------------
template <typename t_Allocator, typename t_Type, uint32_t t_Capacity>
class FixedSimpleMap
{
  using EntryType = Entry<t_Type>;

public:
  FixedSimpleMap()
  {
    invalidateHash();
    ASSERT(t_Capacity < MapEnd,
           "Hashed to max uint64_t, this isn't good... Why would you even make "
           "this large of a hash map?");
  }
  //-----------------------------------------------------------------------------
  void set(const uint64_t m_Key, const t_Type& p_Value)
  {
    ASSERT(m_Data.size() <= m_Data.m_Capacity, "Fixed map is full");

    uint32_t dataIndex = findOrMake(m_Key);
    m_Data[dataIndex].value = p_Value;
  }
  //-----------------------------------------------------------------------------
  const bool has(const uint64_t p_Key) const
  {
    return find(p_Key).dataIndex != MapEnd;
  }
  //-----------------------------------------------------------------------------
  t_Type& get(const uint64_t p_Key, t_Type& p_Default)
  {
    FindResult res = find(p_Key);
    if (res.dataIndex != MapEnd)
    {
      return m_Data[res.dataIndex].value;
    }

    return p_Default;
  }
  //-----------------------------------------------------------------------------
  void remove(const uint64_t p_Key)
  {
    findAndErase(p_Key);
  }
  //-----------------------------------------------------------------------------
  void clear()
  {
    invalidateHash();
    t_Capacity.clear();
  }
  //-----------------------------------------------------------------------------
private:
  void invalidateHash()
  {
    m_Hash.resize(m_Hash.m_Capacity);
    for (uint32_t i = 0u; i < m_Hash.m_Size; i++)
    {
      m_Hash[i] = MapEnd;
    }
  }
  //-----------------------------------------------------------------------------
  FindResult find(const uint64_t p_Key)
  {
    FindResult findResult;

    if (m_Hash.empty())
      return findResult;

    findResult.hashIndex = p_Key % m_Hash.size();
    findResult.dataIndex = m_Hash[findResult.hashIndex];

    while (findResult.dataIndex != MapEnd)
    {
      if (m_Data[findResult.dataIndex].key == p_Key)
      {
        // dataIndex isn't MapEnd
        return findResult;
      }

      findResult.dataPrevIndex = findResult.dataIndex;
      findResult.dataIndex = m_Data[findResult.dataIndex].next;
    }

    // dataIndex is always MapEnd
    return findResult;
  }
  //-----------------------------------------------------------------------------
  uint32_t addEntry(const uint64_t p_Key)
  {
    EntryType entry;
    entry.key = p_Key;
    entry.next = MapEnd;
    // Call constructor?
    const uint32_t dataIndex = m_Data.size();
    m_Data.push_back(entry);
    return dataIndex;
  }
  //-----------------------------------------------------------------------------
  void erase(const FindResult& p_FindResult)
  {
    // Manage hash array
    // If first entry in the linked list
    if (p_FindResult.dataPrevIndex == MapEnd)
    {
      ASSERT(m_Data[p_FindResult.dataIndex].next == MapEnd,
             "Not supposed to happen");
      m_Hash[p_FindResult.hashIndex] = m_Data[p_FindResult.dataIndex].next;
    }
    else
    {
      // Not first entry
      m_Data[p_FindResult.dataPrevIndex].next =
          m_Data[p_FindResult.dataIndex].next;
    }

    // Call destructor
    // TODO: not sure if detructor is called
    m_Data[p_FindResult.dataIndex].value.~t_Type();

    // Swap entries
    m_Data[p_FindResult.dataIndex] = m_Data[m_Data.size() - 1u];
    FindResult last = find(m_Data[p_FindResult.dataIndex].key);
    if (last.dataPrevIndex == MapEnd)
    {
      m_Hash[last.hashIndex] = p_FindResult.dataIndex;
    }
    else
    {
      m_Data[last.dataPrevIndex].next = p_FindResult.dataIndex;
    }
  }
  //-----------------------------------------------------------------------------
  uint32_t findOrMake(const uint64_t p_Key)
  {
    const FindResult findResult = find(p_Key);
    if (findResult.dataIndex != MapEnd)
    {
      return findResult.dataIndex;
    }

    const uint32_t dataIndex = addEntry(p_Key);
    // If the current DataIndex is not used
    if (findResult.dataPrevIndex == MapEnd)
    {
      m_Hash[findResult.hashIndex] = dataIndex;
    }
    else
    {
      // If it's used, add it to the linked list
      m_Data[findResult.dataPrevIndex].next = dataIndex;
    }

    m_Data[dataIndex].next = findResult.dataIndex;

    return dataIndex;
  }
  //-----------------------------------------------------------------------------
  void findAndErase(const uint64_t p_Key)
  {
    const FindResult res = find(p_Key);
    if (res.dataIndex != MapEnd)
    {
      erase(res);
    }
  }
  //-----------------------------------------------------------------------------
  FixedSimpleArray<t_Allocator, uint32_t, t_Capacity> m_Hash;
  FixedSimpleArray<t_Allocator, EntryType, t_Capacity> m_Data;
  //-----------------------------------------------------------------------------
};
}; // namespace Container
}; // namespace Foundation
