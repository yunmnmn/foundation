#pragma once

#include <Util/Logger.h>

// Foundation::Container::MultipleProducerLockFreeQueue<std::string, 128u>
//    Foundation::Log::LogData::ms_LogQueue;
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
//// ConsoleModule
////---------------------------------------------------------------------------------//
// template <typename t_TypeInfo>
// void ConsoleModule<t_TypeInfo>::write(const LogEntry& p_LogEntry)
//{
//  std::cout << p_LogEntry.log;
//}
////---------------------------------------------------------------------------------//
//// DebugOutputModule
////---------------------------------------------------------------------------------//
// template <typename t_TypeInfo>
// void DebugOutputModule<t_TypeInfo>::write(const LogEntry& p_LogEntry)
//{
//}
////---------------------------------------------------------------------------------//
// template <typename t_TypeInfo>
// void FileModule<t_TypeInfo>::write(const LogEntry& p_LogEntry)
//{
//}
////---------------------------------------------------------------------------------//
//
//
////---------------------------------------------------------------------------------//
////---------------------------------------------------------------------------------//
////---------------------------------------------------------------------------------//
//}; // namespace Log
//}; // namespace Foundation
