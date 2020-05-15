//
// Created by carlo on 12.04.20.
//

#ifndef LIB_INCLUDE_LOGGER_H_
#define LIB_INCLUDE_LOGGER_H_

#include <string>
#include <functional>
#include <iosfwd>

using LogCallback = std::function<void(std::string message)>;

class Logger{
 public:
  enum LogType{INFO, WARNING, ERROR};
  enum Verbosity{NORMAL = 0, HIGHER = 1, HIGHEST = 2};

 private:
  LogCallback callback;
  std::string uuid;
  Verbosity verbosity;
  std::ostream & terminal;
  static std::string generateLogEntry(Logger::LogType logType,
                                      std::string message, std::string uuid);
  static std::string generateTerminalLogEntry(Logger::LogType logType,
      std::string message);
  static std::string logTypeToString(Logger::LogType logType);
  static std::string getTimeString();
 public:
  Logger(LogCallback callback, Verbosity v, std::string uuid,
      std::ostream& out);
  void push(LogType logType, std::string message, Verbosity level);
  void pushToTerminal(LogType logType, std::string message, Verbosity level);
  void setVerbosity(Verbosity v);
  static void pushToTerminal(LogType logType, std::string message,
      std::ostream & terminal);
};
#endif  // LIB_INCLUDE_LOGGER_H_
