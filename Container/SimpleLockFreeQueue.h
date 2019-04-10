#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <atomic>

#include <Container/ContainerTypes.h>
#include <Container/SimpleFixedArray.h>
#include <Util/Assert.h>

namespace Foundation
{
namespace Container
{
//-----------------------------------------------------------------------------
template <typename t_Allocator, typename t_Type, uint32_t t_Capacity>
struct SimpleLockFreeFixedQueue
{
  SimpleLockFreeFixedQueue()
  {
    m_Data.resize(Capacity);
    m_Tail.store(0, std::memory_order_relaxed);
    m_Head.store(0, std::memory_order_relaxed);
  }

  ~SimpleLockFreeFixedQueue()
  {
  }

  bool isFull()
  {
    return size() >= capacity();
  }

  bool isEmpty()
  {
    return size() == 0u;
  }

  uint32_t size()
  {
    return m_Tail.load(std::memory_order_acquire) -
           m_Head.load(std::memory_order_acquire);
  }

  uint32_t capacity()
  {
    return Capacity;
  }

  // Single producer
  bool push(t_Type& p_Entry)
  {
    ASSERT(!isFull(), "Queue is full");

    uint32_t tail = m_Tail + 1u;
    m_Data[tail % Capacity] = p_Entry;

    m_Tail.fetch_add(1u, std::memory_order_acquire);
    return true;
  }

  // Multiple consumers
  bool pop(t_Type& p_Entry)
  {
    uint32_t tail = m_Tail.load(std::memory_order_acquire);
    uint32_t head = m_Head.load(std::memory_order_acquire);

    ASSERT(head < tail, "Popping too many");

    while (head < tail)
    {
      if (!m_Head.compare_exchange_weak(head, head + 1u,
                                        std::memory_order_acquire))
      {
        head = m_Head.load(std::memory_order_acquire);
        break;
      }
      else
      {
        p_Entry = m_Data[head % Capacity];
        return true;
      }
    }

    return false;
  }

private:
  ::Foundation::Container::SimpleFixedArray<t_Allocator, t_Type, t_Capacity>
      m_Data;
  std::atomic<uint32_t> m_Head;
  std::atomic<uint32_t> m_Tail;
  const uint32_t Capacity = t_Capacity;
  // ____cacheline_aligned
};
//-----------------------------------------------------------------------------
}; // namespace Container
}; // namespace Foundation
