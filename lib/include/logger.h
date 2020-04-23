//
// Created by carlo on 12.04.20.
//

#ifndef LIB_INCLUDE_LOGGER_H_
#define LIB_INCLUDE_LOGGER_H_

#include <string>

class Logger{
 public:
  enum LogType{INFO, WARNING, ERROR};
  enum Verbosity{NORMAL, HIGH, HIGHEST};
  static std::string generateLogEntry(Logger::LogType logType,
      std::string message, std::string uuid);
};
#endif  // LIB_INCLUDE_LOGGER_H_
