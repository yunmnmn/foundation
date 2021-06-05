#pragma once

#include <Parallel/SpinAndSleepLock.h>

namespace Foundation
{
SpinAndSleepLock::SpinAndSleepLock(uint32_t p_spinCountBeforeSleep)
{
   InitializeCriticalSectionAndSpinCount(&m_criticalSection, p_spinCountBeforeSleep);
}

SpinAndSleepLock::~SpinAndSleepLock()
{
   DeleteCriticalSection(&m_criticalSection);
}

void SpinAndSleepLock::Lock()
{
   EnterCriticalSection(&m_criticalSection);
}

void SpinAndSleepLock::Unlock()
{
   LeaveCriticalSection(&m_criticalSection);
}
}; // namespace Foundation
