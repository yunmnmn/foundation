//#include "Logger.h"
//
// namespace Foundation
//{
// namespace Log
//{
////---------------------------------------------------------------------------------//
// struct LogEntry
//{
//  const std::string log;
//  const Severity logSeverity;
//  // time
//  const char* const fileName;
//  const char* const functionName;
//  const long line;
//};
////---------------------------------------------------------------------------------//
// template <typename t_Log, typename t_LogTypeInfo>
// void BaseLog<t_Log, t_LogTypeInfo>::write(const LogEntry& p_LogEntry);
// template <typename t_Log, typename t_LogTypeInfo>
// std::vector<LogModuleBase<t_LogTypeInfo>*>
//    BaseLog<t_Log, t_LogTypeInfo>::ms_logModuleList;
////---------------------------------------------------------------------------------//
//// ConsoleModule
////---------------------------------------------------------------------------------//
// template <typename t_LogTypeInfo>
// void ConsoleModule<t_LogTypeInfo>::write(const LogEntry& p_LogEntry)
//{
//  std::cout << p_LogEntry.log;
//}
////---------------------------------------------------------------------------------//
//// DebugOutputModule
////---------------------------------------------------------------------------------//
// template <typename t_LogTypeInfo>
// void DebugOutputModule<t_LogTypeInfo>::write(const LogEntry& p_LogEntry)
//{
//}
////---------------------------------------------------------------------------------//
// template <typename t_LogTypeInfo>
// void FileModule<t_LogTypeInfo>::write(const LogEntry& p_LogEntry)
//{
//}
////---------------------------------------------------------------------------------//
////---------------------------------------------------------------------------------//
////---------------------------------------------------------------------------------//
////---------------------------------------------------------------------------------//
//}; // namespace Log
//}; // namespace Foundation
