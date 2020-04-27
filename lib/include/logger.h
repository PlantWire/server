//
// Created by carlo on 12.04.20.
//

#ifndef LIB_INCLUDE_LOGGER_H_
#define LIB_INCLUDE_LOGGER_H_

#include <string>
#include <functional>

using LogCallback = std::function<void(std::string message)>;

class Logger{
 public:
  enum LogType{INFO, WARNING, ERROR};
  enum Verbosity{NORMAL = 0, HIGHER = 1, HIGHEST = 2};

 private:
  LogCallback callback;
  std::string uuid;
  Verbosity verbosity;
  static std::string generateLogEntry(Logger::LogType logType,
                                      std::string message, std::string uuid);

 public:
  Logger(LogCallback callback, Verbosity v, std::string uuid);
  void push(LogType logType, std::string message, Verbosity level);
  void setVerbosity(Verbosity v);
};
#endif  // LIB_INCLUDE_LOGGER_H_
