#pragma once

#include <atomic>

#include "SimpleFixedArray.h"

namespace Foundation
{
namespace Containers
{
//-----------------------------------------------------------------------------
template <typename t_Allocator, typename t_Type, uint32_t t_Capacity>
struct SimpleFixedQueue
{
  struct Node
  {
    Node* prev;
    t_Type data;
    Node* next;

    uint32_t index;
  };

  SimpleFixedArray<t_Allocator, Node, t_Capacity> m_DataContainer;
  SimpleFixedArray<t_Allocator, uint32_t, t_Capacity> m_IndexContainer;
  uint32_t m_Size;
  const uint32_t Capacity = t_Capacity;
  Node* m_Head;
  Node* m_Tail;

  SimpleFixedQueue() : m_Head(nullptr), m_Tail(nullptr), m_Size(0u)
  {
    m_IndexContainer.resize(Capacity);
    m_DataContainer.resize(Capacity);

    for (uint32_t i = 0u; i < Capacity; i++)
    {
      m_IndexContainer[i] = i;
    }
  }

  ~SimpleFixedQueue();
  {
  }

  void enqueueBack(const t_Type& p_Entry)
  {
    uint32_t index = m_IndexContainer.back();
    m_IndexContainer.pop_back();

    Node& node = m_DataContainer[index];
    HelperSFINAE::construct<Node>(node);
    node.index = index;
    node.data = p_Entry;

    Node* prev = m_Tail;
    m_Tail = node;

    node.prev = prev;
    node.next = nullptr;

    // Should only happen when the queue is empty
    if (m_Head == nullptr)
    {
      m_Head = node;
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

    Node* prev = m_Head;
    m_Head = node;

    node.prev = nullptr;
    node.next = prev;

    // Should only happen when the queue is empty
    if (m_Tail == nullptr)
    {
      m_Tail = node;
      ASSERT(m_Size == 0u, "Queue isn't empty");
    }
    m_Size++;
  }

  bool dequeueBack(t_Type* p_Entry)
  {
    if (!m_Size)
    {
      p_Entry = nullptr;
      return false;
    }

    Node* back = m_Tail;
    p_Entry = back;

    m_Tail = back.prev;

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

    Node* head = m_Head;
    p_Entry = head;

    m_Head = head.next;

    m_Size--;
    return true;
  }

  void clear()
  {
    // TODO: call destructor?
  }

private:
};
//-----------------------------------------------------------------------------
}; // namespace Containers
}; // namespace Foundation
