#pragma once

#include <Container/ContainerTypes.h>

namespace Foundation
{
namespace Container
{
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
template <typename t_Allocator, typename t_Type, uint32_t t_Capacity>
class SimpleFixedMap
{
  using EntryType = Entry<t_Type>;

public:
  SimpleFixedMap()
  {
    _invalidateHash();
    ASSERT(t_Capacity < MapEnd,
           "Hashed to max uint64_t, this isn't good... Why would you even make "
           "this large of a hash map?");
  }
  //-----------------------------------------------------------------------------
  void set(const uint64_t m_Key, const t_Type& p_Value)
  {
    ASSERT(m_Data.size() <= t_Capacity, "Fixed map is full");

    uint32_t dataIndex = _findOrMake(m_Key);
    m_Data[dataIndex].value = p_Value;
  }
  //-----------------------------------------------------------------------------
  const bool has(const uint64_t p_Key) const
  {
    return _find(p_Key).dataIndex != MapEnd;
  }
  //-----------------------------------------------------------------------------
  t_Type& get(const uint64_t p_Key, t_Type& p_Default)
  {
    FindResult res = _find(p_Key);
    if (res.dataIndex != MapEnd)
    {
      return m_Data[res.dataIndex].value;
    }

    return p_Default;
  }
  //-----------------------------------------------------------------------------
  void remove(const uint64_t p_Key)
  {
    _findAndErase(p_Key);
  }
  //-----------------------------------------------------------------------------
  void clear()
  {
    _invalidateHash();
    t_Capacity.clear();
  }
  //-----------------------------------------------------------------------------
private:
  void _invalidateHash()
  {
    m_Hash.resize(t_Capacity);
    for (uint32_t i = 0u; i < m_Hash.size(); i++)
    {
      m_Hash[i] = MapEnd;
    }
  }
  //-----------------------------------------------------------------------------
  FindResult _find(const uint64_t p_Key)
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
  uint32_t _addEntry(const uint64_t p_Key)
  {
    EntryType entry;
    entry.key = p_Key;
    entry.next = MapEnd;
    const uint32_t dataIndex = m_Data.size();
    m_Data.push_back(entry);
    return dataIndex;
  }
  //-----------------------------------------------------------------------------
  void _erase(const FindResult& p_FindResult)
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
    // TODO: not sure if destructor is called
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
  uint32_t _findOrMake(const uint64_t p_Key)
  {
    const FindResult findResult = _find(p_Key);
    if (findResult.dataIndex != MapEnd)
    {
      return findResult.dataIndex;
    }

    const uint32_t dataIndex = _addEntry(p_Key);
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
  void _findAndErase(const uint64_t p_Key)
  {
    const FindResult res = _find(p_Key);
    if (res.dataIndex != MapEnd)
    {
      _erase(res);
    }
  }
  //-----------------------------------------------------------------------------
  SimpleFixedArray<t_Allocator, uint32_t, t_Capacity> m_Hash;
  SimpleFixedArray<t_Allocator, EntryType, t_Capacity> m_Data;
  //-----------------------------------------------------------------------------
};
//-----------------------------------------------------------------------------
}; // namespace Container
}; // namespace Foundation
