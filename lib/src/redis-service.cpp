#include "../include/redis-service.h"

#include <chrono>

RedisService::RedisService(std::string host, uint16_t port,
    std::string password, Logger &logger) : logger{logger} {
  this->connection_options.host = host;
  this->connection_options.port = port;
  this->connection_options.password = password;
  this->connection_options.socket_timeout = std::chrono::milliseconds{200};
  this->redis = new sw::redis::Redis{this->connection_options};
  this->subscriber = new sw::redis::Subscriber{std::move(this->redis->subscriber())};
  this->subscriberThread = new std::thread(&RedisService::subscriberConsume, this);
}

RedisService::~RedisService() {
  {
    std::lock_guard<std::mutex> lck{subscriberLock};
    this->stopped = true;
  }
  this->subscriberThread->join();
  delete(this->subscriber);
  delete(this->redis);
}

void RedisService::createLogEntry(LogType logType, std::string message,
    Verbosity v, bool terminal) {
  if (terminal) {
    this->logger.pushToTerminal(logType, "[Redis] " + message, v);
  } else {
    this->logger.push(logType, "[Redis] " + message, v);
  }
}

void RedisService::push(std::string channel, std::string data) {
  this->redis->publish(channel, data);
}

void RedisService::subscribe(std::string channel,
    subscribe_callback_t callback) {
  this->subscriber->subscribe(channel);
  this->subscriber->on_message(callback);
}

void RedisService::subscriberConsume() {
  while (true) {
    {
      std::lock_guard<std::mutex> lck{subscriberLock};
      if(stopped) {
        return;
      }
      try {
        this->subscriber->consume();
        this->redis->publish("pwire-frontend", "Yooo");
      } catch (sw::redis::TimeoutError const &e) {
        // Try again.
        continue;
      }  catch (sw::redis::Error const &e) {
        this->createLogEntry(LogType::ERROR,
                             "Redis subscriber error. Error: " + std::string{e.what()},
                             Verbosity::NORMAL);
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}
