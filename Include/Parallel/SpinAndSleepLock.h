#pragma once

#include <cstdint>

class SpinAndSleepLock
{
public:
	SpinAndSleepLock(uint32_t p_spinCountBeforeSleep);
	~SpinAndSleepLock() = default;

	SpinAndSleepLock(const SpinAndSleepLock_Win& other) = delete;            // copy constructor
	SpinAndSleepLock& operator=(const SpinAndSleepLock_Win& other) = delete; // copy assignment

	void Lock();
	void Unlock();

private:
	RTL_CRITICAL_SECTION m_criticalSection;
	uint32_t m_spinCountBeforeSleep = 0u;
};
}; // namespace Foundation
