#include "../include/logger.h"

std::string Logger::generateLogEntry(Logger::LogType logType,
    std::string message, std::string uuid) {
  std::string logTypeString{};

  switch (logType) {
    case info:
      logTypeString = "info";
      break;
    case warning:
      logTypeString = "warn";
      break;
    case error:
      logTypeString = "err";
      break;
  }

  std::string data {
    "{"
      "\"Type\": \"logging\","
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
