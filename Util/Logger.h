#pragma once

#include <map>

#include <iostream>
#include <stdbool.h>
#include <util/Util.h>
#include <Container/ContainerTypes.h>
#include <util/HashName.h>

#include <string>
#include <vector>

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
  const std::string log;
  const Severity logSeverity;
  // time
  const char* const fileName;
  const char* const functionName;
  const long line;
};
//---------------------------------------------------------------------------------//
template <typename t_LogTypeInfo> struct LogModuleBase
{
  const t_LogTypeInfo logType;
  virtual void write(const LogEntry& p_LogEntry) = 0;
};
//---------------------------------------------------------------------------------//
template <typename t_LogTypeInfo>
struct ConsoleModule final : LogModuleBase<t_LogTypeInfo>
{
  ConsoleModule()
  {
  }
  void write(const LogEntry& p_LogEntry) override
  {
    std::cout << p_LogEntry.log;
  }
};
//---------------------------------------------------------------------------------//
template <typename t_LogTypeInfo>
struct DebugOutputModule final : LogModuleBase<t_LogTypeInfo>
{
  void write(const LogEntry& p_LogEntry) override
  {
  }
};
//------------------------------------------------------------------------------//
template <typename t_LogTypeInfo>
struct FileModule final : LogModuleBase<t_LogTypeInfo>
{
  void write(const LogEntry& p_LogEntry) override
  {
    // - Output different files for every log type
    // - Also output into one file
  }
};
//------------------------------------------------------------------------------//
template <typename t_Log, typename t_LogTypeInfo> struct BaseLog
{
  static std::vector<LogModuleBase<t_LogTypeInfo>*> ms_logModuleList;

  static void addModule(LogModuleBase<t_LogTypeInfo>* p_Module)
  {
    ms_logModuleList.push_back(p_Module);
  }

  static void write(const LogEntry& p_LogEntry)
  {
    callOnce([]() { t_Log::init(); });

    for (LogModuleBase<t_LogTypeInfo>* logModule : ms_logModuleList)
    {
      logModule->write(p_LogEntry);
    }
  }
};
//---------------------------------------------------------------------------------//
template <typename t_Log, typename t_LogTypeInfo>
void BaseLog<t_Log, t_LogTypeInfo>::write(const LogEntry& p_LogEntry);
template <typename t_Log, typename t_LogTypeInfo>
std::vector<LogModuleBase<t_LogTypeInfo>*>
    BaseLog<t_Log, t_LogTypeInfo>::ms_logModuleList;
//---------------------------------------------------------------------------------//
} // namespace Log
} // namespace Foundation
