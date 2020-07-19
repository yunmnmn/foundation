#pragma once

#include <atomic>

namespace Foundation
{
class SpinLock
{
   SpinLock() = default;
   ~SpinLock() = default;

   SpinLock(const SpinLock& other) = delete;
   SpinLock& operator=(const SpinLock& other) = delete;

   void Lock();
   void Unlock();

 private:
   std::atomic_flag m_flag;
};
} // namespace Foundation
