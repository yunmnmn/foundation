#pragma once

#include <inttypes.h>
#include <stdbool.h>

#define YH_WINBASEAPI __declspec(dllimport)
#define YH_WINAPI __stdcall
//---------------------------------------------------------------------------------//
using YH_BOOL      = int32_t;
using YH_HANDLE    = void*;
using YH_DWORD     = uint32_t;
using YH_WORD      = uint16_t;
using YH_DWORD64   = int64_t;
using YH_ULONG_PTR = uint64_t;
//---------------------------------------------------------------------------------//
// typedef YH_DWORD(YH_WINAPI* TThreadStartFunc)(void* lpThreadParameter);
using TThreadStartFunc = YH_DWORD(YH_WINAPI*)(void* lpThreadParameter);
// typedef void(YH_WINAPI* TFiberStartFunc)(void* lpFiberParameter);
using TFiberStartFunc = void(YH_WINAPI*)(void* lpThreadParameter);
//---------------------------------------------------------------------------------//
struct YH_SYSTEM_INFO
{
  uint8_t _unused_01[4];
  YH_DWORD dwPageSize;
  void* _unused_02[3];
  YH_DWORD dwNumberOfProcessors;
  uint8_t _unused_03[12];
};

struct __declspec(align(16)) YH_CONTEXT
{
  uint8_t _unused_01[48];
  YH_DWORD ContextFlags;
  uint8_t _unused_02[100];
  YH_DWORD64 Rsp;
  uint8_t _unused_03[88];
  YH_DWORD64 Rip;
  uint8_t _unused_04[976];
};

struct YH_LARGE_INTEGER
{
  int64_t QuadPart;
};

struct YH_CRITICAL_SECTION
{
  uint8_t _unused[24];
};
//---------------------------------------------------------------------------------//
// static_assert(__alignof(YH_CONTEXT) == 16u,
//              "YH_CONTEXT align requirements must be 16 bytes");
//---------------------------------------------------------------------------------//
#define YH_CREATE_SUSPENDED (0x00000004)
#define YH_FIBER_FLAG_FLOAT_SWITCH (0x1)

#define YH_THREAD_PRIORITY_HIGHEST (2)
#define YH_THREAD_PRIORITY_NORMAL (0)
#define YH_THREAD_PRIORITY_LOWEST (-2)
//---------------------------------------------------------------------------------//
extern "C"
{
  YH_WINBASEAPI YH_DWORD YH_WINAPI
  SetThreadIdealProcessor(YH_HANDLE hThread, YH_DWORD dwIdealProcessor);
  YH_WINBASEAPI YH_BOOL YH_WINAPI SetThreadPriority(YH_HANDLE hThread,
                                                    int nPriority);
  YH_WINBASEAPI YH_HANDLE YH_WINAPI
  CreateThread(void* lpThreadAttributes, size_t dwStackSize,
               TThreadStartFunc lpStartAddress, void* lpParameter,
               YH_DWORD dwCreationFlags, YH_DWORD* lpThreadId);
  YH_WINBASEAPI YH_DWORD YH_WINAPI
  SetThreadIdealProcessor(YH_HANDLE hThread, YH_DWORD dwIdealProcessor);
  YH_WINBASEAPI YH_BOOL YH_WINAPI SetThreadPriority(YH_HANDLE hThread,
                                                    int nPriority);
  YH_WINBASEAPI YH_DWORD YH_WINAPI ResumeThread(YH_HANDLE hThread);

  YH_WINBASEAPI bool YH_WINAPI InitializeCriticalSectionAndSpinCount(
      YH_CRITICAL_SECTION* lpCriticalSection, YH_DWORD dwSpinCount);
  YH_WINBASEAPI void YH_WINAPI
  DeleteCriticalSection(YH_CRITICAL_SECTION* lpCriticalSection);
  YH_WINBASEAPI void YH_WINAPI
  EnterCriticalSection(YH_CRITICAL_SECTION* lpCriticalSection);
  YH_WINBASEAPI void YH_WINAPI
  LeaveCriticalSection(YH_CRITICAL_SECTION* lpCriticalSection);

  YH_WINBASEAPI void YH_WINAPI DeleteFiber(void* lpFiber);
  YH_WINBASEAPI void* YH_WINAPI ConvertThreadToFiberEx(void* lpParameter,
                                                       YH_DWORD dwFlags);
  YH_WINBASEAPI void* YH_WINAPI CreateFiber(size_t dwStackSize,
                                            TFiberStartFunc lpStartAddress,
                                            void* lpParameter);
  YH_WINBASEAPI void YH_WINAPI SwitchToFiber(void* lpFiber);
  YH_WINBASEAPI YH_BOOL YH_WINAPI IsThreadAFiber();

  YH_WINBASEAPI void YH_WINAPI GetSystemInfo(YH_SYSTEM_INFO* lpSystemInfo);
  YH_WINBASEAPI void YH_WINAPI Sleep(YH_DWORD dwMilliseconds);
}
