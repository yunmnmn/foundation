#pragma once

#include <Util/Macro.h>

#include <atomic>

namespace Foundation
{
class SpinLock
{
 public:
   SpinLock() = default;
   ~SpinLock() = default;

   NO_COPY_ASSIGN(SpinLock);

   void Lock();
   void Unlock();

 private:
   std::atomic_flag m_flag;
};
} // namespace Foundation
