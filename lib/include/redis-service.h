#ifndef LIB_INCLUDE_REDIS_SERVICE_H_
#define LIB_INCLUDE_REDIS_SERVICE_H_

#include <cpp_redis/cpp_redis>
#include <tacopie/tacopie>
#include <string>
#include "./logger.h"

using connect_state = cpp_redis::connect_state;
using Verbosity = Logger::Verbosity;
using LogType = Logger::LogType;

typedef std::function<void(const std::string &channel,
                           const std::string &msg)> subscribe_callback_t;

class RedisService {
 private:
  cpp_redis::subscriber sub;
  cpp_redis::client client;
  Logger &logger;

  void clientConnect();
  void subConnect();
  void createLogEntry(LogType logType, std::string message, Verbosity v);
 public:
  explicit RedisService(Logger &logger);
  ~RedisService();
  void push(std::string channel, std::string data);
  void subscribe(std::string channel, subscribe_callback_t callback);
};


#endif  // LIB_INCLUDE_REDIS_SERVICE_H_