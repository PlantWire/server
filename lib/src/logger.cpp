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
