#ifndef LIB_INCLUDE_REDIS_SERVICE_H_
#define LIB_INCLUDE_REDIS_SERVICE_H_

#include <string>
#include <mutex>
#include <thread>
#include <sw/redis++/redis++.h>
#include <atomic>

#include "./logger.h"

using Verbosity = Logger::Verbosity;
using LogType = Logger::LogType;

typedef std::function<void(const std::string &channel,
                           const std::string &msg)> subscribe_callback_t;

class RedisService {
 private:
  static constexpr char REDIS_NO_PASSWORD_SET_ERROR[] =
      "ERR Client sent AUTH, but no password is set";

  sw::redis::ConnectionOptions connection_options;
  sw::redis::Redis * redis;
  sw::redis::Subscriber * subscriber;
  Logger &logger;
  std::mutex subscriberLock{};
  std::thread *subscriberThread;
  std::atomic<bool> stopped{false};

  void createLogEntry(LogType logType, std::string message, Verbosity v,
      bool terminal = false);
  void subscriberConsume();
 public:
  RedisService(std::string host, uint16_t port, std::string password,
      Logger &logger);
  ~RedisService();
  void push(std::string channel, std::string data);
  void subscribe(std::string channel, subscribe_callback_t callback);

};


#endif  // LIB_INCLUDE_REDIS_SERVICE_H_
