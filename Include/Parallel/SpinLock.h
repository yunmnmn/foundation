#pragma once

#include <Util/Macro.h>

#include <atomic>

namespace Foundation
{
class SpinLock
{
   SpinLock() = default;
   ~SpinLock() = default;

   NO_COPY_ASSIGN(SpinLock);

   void Lock();
   void Unlock();

 private:
   std::atomic_flag m_flag;
};

template <typename t_lock>
class LockScopeGuard
{
   SpinLockScopeGuard()
   {
      m_lock.Lock();
   }

   ~SpinLockScopeGuard()
   {
      m_lock.Unlock();
   }

   t_lock m_lock;
};
} // namespace Foundation
