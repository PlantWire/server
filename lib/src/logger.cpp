#include <stdexcept>
#include <ostream>
#include <ctime>
#include "../include/logger.h"

std::string Logger::logTypeToString(Logger::LogType logType) {
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

  return logTypeString;
}

std::string Logger::getTimeString() {
  time_t now = time(0);
  tm ltm{};
  localtime_r(&now, &ltm);

  std::string day;
  std::string month;
  std::string year;

  std::string hour;
  std::string minute;
  std::string second;

  if (ltm.tm_mon < 10) {
    month = "0" + std::to_string(1 + ltm.tm_mon);
  } else {
    month = std::to_string(1 + ltm.tm_mon);
  }

  if (ltm.tm_mday < 10) {
    day = "0" + std::to_string(ltm.tm_mday);
  } else {
    day = std::to_string(ltm.tm_mday);
  }

  if (ltm.tm_hour < 10) {
    hour = "0" + std::to_string(ltm.tm_hour);
  } else {
    hour = std::to_string(ltm.tm_hour);
  }

  if (ltm.tm_min < 10) {
    minute = "0" + std::to_string(ltm.tm_min);
  } else {
    minute = std::to_string(ltm.tm_min);
  }

  if (ltm.tm_sec < 10) {
    second = "0" + std::to_string(ltm.tm_sec);
  } else {
    second = std::to_string(ltm.tm_sec);
  }


  std::string date {day + "-" + month + "-" +
    std::to_string(1900 + ltm.tm_year) + " " + hour + ":" + minute + ":" +
    second};

  return date;
}

std::string Logger::generateLogEntry(Logger::LogType logType,
    std::string message, std::string uuid) {
  std::string logTypeString = Logger::logTypeToString(logType);

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

std::string Logger::generateTerminalLogEntry(Logger::LogType logType,
    std::string message) {
  std::string logTypeString = Logger::logTypeToString(logType);
  std::string data {Logger::getTimeString() +
    " (" + logTypeString + ") " + message};

  return data;
}

Logger::Logger(LogCallback callback, Verbosity v, std::string uuid,
    std::ostream& terminal) : callback{callback}, verbosity{v}, uuid(uuid),
    terminal{terminal} {
}

void Logger::push(LogType logType, std::string message, Verbosity level) {
  if (level <= this->verbosity) {
    std::string logEntry =
        Logger::generateLogEntry(logType, message, this->uuid);
    this->callback(logEntry);
  }
}

void Logger::pushToTerminal(LogType logType, std::string message,
    Verbosity level) {
  if (level <= this->verbosity) {
    Logger::pushToTerminal(logType, message, this->terminal);
  }
}

void Logger::setVerbosity(Verbosity v) {
  this->verbosity = v;
}

void Logger::pushToTerminal(LogType logType, std::string message,
    std::ostream & terminal) {
  terminal << Logger::generateTerminalLogEntry(logType, message) << std::endl;
}

