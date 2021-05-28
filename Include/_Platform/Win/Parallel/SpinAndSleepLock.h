#pragma once

#include <cstdint>

#include <Windows.h>
#include <Util/Macro.h>

namespace Foundation
{
class SpinAndSleepLock
{
 public:
   SpinAndSleepLock(uint32_t p_spinCountBeforeSleep);
   ~SpinAndSleepLock();

   NO_COPY_ASSIGN(SpinAndSleepLock);

   void Lock();
   void Unlock();

 private:
   RTL_CRITICAL_SECTION m_criticalSection;
   uint32_t m_spinCountBeforeSleep = 0u;
};
}; // namespace Foundation
