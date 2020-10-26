#pragma once

#include <Util/Util.h>
#include <Util/HashName.h>

#include <Std/string_bootstrap.h>

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

namespace Foundation
{
namespace Util
{
namespace Log
{
enum class Severity : uint8_t
{
   Info,
   Debug,
   Warning,
   Error
};

struct LogEntry
{
   LogEntry(Std::string_bootstrap p_Log, Severity p_LogSeverity, const char* const p_FileName, const char* const p_FunctionName,
            long p_Line)
       : log(p_Log), logSeverity(p_LogSeverity), fileName(p_FileName), functionName(p_FunctionName), line(p_Line)
   {
   }

   const Std::string_bootstrap log;
   Severity logSeverity;
   // time
   const char* const fileName;
   const char* const functionName;
   long line;
};

// Implement the modules correctly
template <typename t_TypeInfo>
struct ConsoleModule
{
   static void Write(const LogEntry& p_logEntry)
   {
      // const Std::string_bootstrap log = Util::SimpleSprintf("%s %s", t_TypeInfo::GetPrefix(), p_logEntry.log.c_str());
      printf("%s %s\n", t_TypeInfo::GetPrefix(), p_logEntry.log.c_str());
   }
};

template <typename t_TypeInfo>
struct DebugOutputModule
{
   static void Write(const LogEntry& p_logEntry)
   {
      UNUSED(p_logEntry);
   }
};

template <typename t_TypeInfo>
struct FileModule
{
   static void Write(const LogEntry& p_logEntry)
   {
      UNUSED(p_logEntry);
   }
};

template <typename... t_LogType>
void LogTupleWrite(const LogEntry& p_logEntry)
{
   // (t_LogType::template write(p_LogEntry), ...);
   (..., t_LogType::template Write(p_logEntry));
}

}; // namespace Log
}; // namespace Util
}; // namespace Foundation
