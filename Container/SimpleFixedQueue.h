#pragma once

#include <atomic>

#include "SimpleFixedArray.h"

namespace Foundation
{
namespace Container
{
//-----------------------------------------------------------------------------
template <typename t_Allocator, typename t_Type, uint32_t t_Capacity>
struct SimpleFixedQueue
{
  struct Node
  {
    uint32_t prev;
    t_Type data;
    uint32_t next;

    uint32_t index;
  };

  SimpleFixedQueue() : m_Head(Empty), m_Tail(Empty), m_Size(0u)
  {
    m_IndexContainer.resize(Capacity);
    m_DataContainer.resize(Capacity);

    for (uint32_t i = 0u; i < Capacity; i++)
    {
      m_IndexContainer[i] = i;
    }
  }

  ~SimpleFixedQueue()
  {
    clear();
  }

  void enqueueBack(const t_Type& p_Entry)
  {
    uint32_t index = m_IndexContainer.back();
    m_IndexContainer.pop_back();

    Node& node = m_DataContainer[index];
    HelperSFINAE::construct<Node>(node);
    node.index = index;
    node.data = p_Entry;

    uint32_t prev = m_Tail;
    m_Tail = node.index;

    node.prev = prev;
    node.next = Empty;

    // Should only happen when the queue is empty
    if (m_Head == Empty)
    {
      m_Head = node.index;
      ASSERT(m_Size == 0u, "Queue isn't empty");
    }
    m_Size++;
  }

  void enqueueFront(const t_Type& p_Entry)
  {
    uint32_t index = m_IndexContainer.back();
    m_IndexContainer.pop_back();

    Node& node = m_DataContainer[index];
    HelperSFINAE::construct<Node>(node);
    node.index = index;
    node.data = p_Entry;

    uint32_t prev = m_Head;
    m_Head = node.index;

    node.prev = Empty;
    node.next = prev;

    // Should only happen when the queue is empty
    if (m_Tail == Empty)
    {
      m_Tail = node;
      ASSERT(m_Size == 0u, "Queue isn't empty");
    }
    m_Size++;
  }

  bool dequeueBack(t_Type& p_Entry)
  {
    if (!m_Size)
    {
      p_Entry = nullptr;
      return false;
    }

    Node& tail = m_DataContainer[m_Tail];
    p_Entry = tail.data;
    m_Tail = tail.prev;

    m_IndexContainer.push_back(tail.index);
    HelperSFINAE::destruct(m_DataContainer + tail.index);
    m_Size--;
    return true;
  }

  bool dequeueFront(t_Type& p_Entry)
  {
    if (!m_Size)
    {
      p_Entry = nullptr;
      return false;
    }

    Node& head = m_DataContainer[m_Head];
    p_Entry = head.data;
    m_Head = head.next;

    m_IndexContainer.push_back(head.index);
    HelperSFINAE::destruct(m_DataContainer + head.index);
    m_Size--;
    return true;
  }

  void clear()
  {
    m_DataContainer.clear();
    m_IndexContainer.clear();
    for (uint32_t i = 0u; i < Capacity; i++)
    {
      m_IndexContainer[i] = i;
    }
    m_Size = 0u;
    m_Head = Empty;
    m_Tail = Empty;
  }

  const uint32_t size() const
  {
    return m_Size;
  }

  friend void copy(SimpleFixedQueue& p_Source, SimpleFixedQueue& p_Dest);

private:
  ::Foundation::Container::SimpleFixedArray<t_Allocator, Node, t_Capacity>
      m_DataContainer;
  ::Foundation::Container::SimpleFixedArray<t_Allocator, uint32_t, t_Capacity>
      m_IndexContainer;
  uint32_t m_Size;
  const uint32_t Capacity = t_Capacity;
  uint32_t m_Head;
  uint32_t m_Tail;
};
//-----------------------------------------------------------------------------
template <typename t_Allocator, typename t_Type, uint32_t t_Capacity>
void copy(SimpleFixedQueue<t_Allocator, t_Type, t_Capacity>& p_Dest,
          SimpleFixedQueue<t_Allocator, t_Type, t_Capacity>& p_Source)
{
  Container::copy(p_Dest.m_DataContainer, p_Source.m_DataContainer);

  p_Dest.m_Size = p_Source.m_Size;
  p_Dest.m_Head = p_Source.m_Head;
  p_Dest.m_Tail = p_Source.m_Tail;
}
//-----------------------------------------------------------------------------
}; // namespace Container
}; // namespace Foundation
