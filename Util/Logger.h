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
struct ConsoleModule
{
  static void write(const LogEntry& p_LogEntry)
  {
    std::cout << p_LogEntry.log;
  }
};
//---------------------------------------------------------------------------------//
struct DebugOutputModule
{
  static void write(const LogEntry& p_LogEntry)
  {
  }
};
//------------------------------------------------------------------------------//
struct FileModule
{
  static void write(const LogEntry& p_LogEntry)
  {
  }
};
//------------------------------------------------------------------------------//
template <typename... t_LogTypePack>
void logTupleWrite(const LogEntry& p_LogEntry)
{
  (t_LogTypePack::write(p_LogEntry), ...);
}
//---------------------------------------------------------------------------------//
}; // namespace Log
}; // namespace Foundation
