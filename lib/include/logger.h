//
// Created by carlo on 12.04.20.
//

#ifndef LIB_INCLUDE_LOGGER_H_
#define LIB_INCLUDE_LOGGER_H_

#include <string>
#include "./redis-service.h"

class Logger{
 public:
  enum LogType{INFO, WARNING, ERROR};
  enum Verbosity{NORMAL = 0, HIGH = 1, HIGHEST = 2};

 private:
  RedisService &redis;
  std::string uuid;
  Verbosity verbosity;
  static std::string generateLogEntry(Logger::LogType logType,
                                      std::string message, std::string uuid);

 public:
  Logger(RedisService &redis, Verbosity v, std::string uuid);
  void push(LogType logType, std::string message, Verbosity level);
  void setVerbosity(Verbosity v);
};
#endif  // LIB_INCLUDE_LOGGER_H_
