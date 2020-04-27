#include <stdexcept>
#include "../include/logger.h"

std::string Logger::generateLogEntry(Logger::LogType logType,
    std::string message, std::string uuid) {
  std::string logTypeString{};

  switch (logType) {
    case INFO:
      logTypeString = "info";
      break;
    case WARNING:
      logTypeString = "warn";
      break;
    case ERROR:
      logTypeString = "err";
      break;
  }

  std::string data {
    "{"
      "\"Type\": \"log\","
      "\"Sender\": \"" + uuid + "\","
      "\"Content\":"
        "{"
          "\"LogType\": \"" + logTypeString + "\","
          "\"Message\": \"" + message + "\""
        "}"
    "}"
  };

  return data;
}

Logger::Logger(LogCallback callback, Verbosity v, std::string uuid) :
    callback{callback}, verbosity{v}, uuid(uuid) {
}

void Logger::push(LogType logType, std::string message, Verbosity level) {
  if (level <= this->verbosity) {
    std::string logEntry =
        Logger::generateLogEntry(logType, message, this->uuid);
    this->callback(logEntry);
  }
}

void Logger::setVerbosity(Verbosity v) {
  this->verbosity = v;
}

