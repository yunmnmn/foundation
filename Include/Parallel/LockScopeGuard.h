#pragma once

#include <Util/Macro.h>

namespace Foundation
{
template <typename t_lock>
class LockScopeGuard
{
 public:
   LockScopeGuard()
   {
      m_lock.Lock();
   }

   ~LockScopeGuard()
   {
      m_lock.Unlock();
   }

   NO_COPY_ASSIGN(LockScopeGuard);

 private:
   t_lock m_lock;
};
} // namespace Foundation
