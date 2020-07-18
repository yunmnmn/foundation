#include <Windows.h>

// TODO: replace this with std calls instead of windows specific ones
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

    void lock()
    {
        EnterCriticalSection(&criticalSection);
    }
    void unlock()
    {
        LeaveCriticalSection(&criticalSection);
    }

    Mutex(const Mutex&) = delete;
    Mutex& operator=(const Mutex&) = delete;

private:
};