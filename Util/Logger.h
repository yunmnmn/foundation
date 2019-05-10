#pragma once

#include <util/Util.h>
#include <Container/ContainerTypes.h>
#include <util/HashName.h>
#include <Container/SimpleLockFreeQueue.h>

#include <string>
#include <iostream>
#include <condition_variable>

#ifdef _MSC_VER
#define GET_FUNC() __FUNCTION__
#elif __GNUC__
#define GET_FUNC() __PRETTY_FUNCTION__
#else
#define GET_FUNC() ""
#endif

#ifdef _MSC_VER
#define GET_FILE() __FILE__
#else
#define GET_FILE() ""
#endif

#define GET_LINE() __LINE__

// TODO: Move everything to the CPP file when everything is more defined
namespace Foundation
{
namespace Log
{
//---------------------------------------------------------------------------------//
static Container::MultipleProducerLockFreeQueue<std::string, 128u> g_LogQueue;
//---------------------------------------------------------------------------------//
enum Severity : uint8_t
{
  Info,
  Debug,
  Warning,
  Error
};
//---------------------------------------------------------------------------------//
struct LogEntry
{
  LogEntry(const char* p_Log, Severity p_LogSeverity,
           const char* const p_FileName, const char* const p_FunctionName,
           long p_Line)
      : log(p_Log), logSeverity(p_LogSeverity), fileName(p_FileName),
        functionName(p_FunctionName), line(p_Line)
  {
  }

  const std::string log;
  Severity logSeverity;
  // time
  const char* const fileName;
  const char* const functionName;
  long line;
};
//---------------------------------------------------------------------------------//
// Implement the modules correctly
template <typename t_TypeInfo> struct ConsoleModule
{
  static void write(const LogEntry& p_LogEntry)
  {
    std::string string = p_LogEntry.log;
    g_LogQueue.push(string);
  }
};
//---------------------------------------------------------------------------------//
template <typename t_TypeInfo> struct DebugOutputModule
{
  static void write(const LogEntry& p_LogEntry)
  {
  }
};
//------------------------------------------------------------------------------//
template <typename t_TypeInfo> struct FileModule
{
  static void write(const LogEntry& p_LogEntry)
  {
  }
};
//------------------------------------------------------------------------------//
inline void processLogQueue()
{
  std::string log;
  // while (g_LogQueue.pop(log))
  while (!g_LogQueue.isEmpty())
  {
    g_LogQueue.pop(log);
    std::cout << log + '\n';
  }
}
//------------------------------------------------------------------------------//
template <typename t_TypeInfo, typename... t_LogType>
void logTupleWrite(const LogEntry& p_LogEntry)
{
  // (t_LogType::template write(p_LogEntry), ...);
  (..., t_LogType::template write(p_LogEntry));
}
//---------------------------------------------------------------------------------//
inline void init()
{
}
//---------------------------------------------------------------------------------//
inline void destroy()
{
}
//---------------------------------------------------------------------------------//
inline bool empty()
{
  return false;
}
//---------------------------------------------------------------------------------//
}; // namespace Log
}; // namespace Foundation
