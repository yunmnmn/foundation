#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <Windows.h>

namespace Foundation
{
//-----------------------------------------------------------------------------
template <typename Functor> void callOnce(Functor functor)
{
  static bool called = false;
  if (!called)
  {
    functor();
    called = true;
  }
}
//-----------------------------------------------------------------------------
class Mutex
{
  RTL_CRITICAL_SECTION criticalSection;

public:
  Mutex()
  {
    InitializeCriticalSectionAndSpinCount(&criticalSection, 128);
  }

  ~Mutex()
  {
    DeleteCriticalSection(&criticalSection);
  }

  Mutex(const Mutex&) = delete;
  Mutex& operator=(const Mutex&) = delete;

  friend class ScopedGuard;

private:
  void Lock()
  {
    EnterCriticalSection(&criticalSection);
  }
  void Unlock()
  {
    LeaveCriticalSection(&criticalSection);
  }
};
//-----------------------------------------------------------------------------
class ScopedGuard
{
  Mutex& mutex;

public:
  ScopedGuard(Mutex& _mutex) : mutex(_mutex)
  {
    mutex.Lock();
  }

  ~ScopedGuard()
  {
    mutex.Unlock();
  }

  void forceUnlock()
  {
    mutex.Unlock();
  }
};
//-----------------------------------------------------------------------------
// Simple Linear congruential generator
class LcgRandom
{
  uint32_t state;

public:
  LcgRandom() : state(2578432553)
  {
  }

  void SetSeed(uint32_t seed)
  {
    state = seed;
  }

  uint16_t Get()
  {
    state = 214013 * state + 2531011;
    uint16_t rnd = (state >> 16);
    return rnd;
  }
};
//-----------------------------------------------------------------------------
} // namespace Foundation
