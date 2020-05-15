#include "../include/redis-service.h"
RedisService::RedisService(std::string host, uint16_t port,
    std::string password, Logger &logger) : logger{logger} {
  this->host = host;
  this->port = port;
  this->password = password;
  this->clientConnect();
  this->subConnect();
}

RedisService::~RedisService() {
  this->sub.disconnect();
  this->client.disconnect();
}

void RedisService::clientConnect() {
  this->client.connect(this->host, this->port,
      [this](const std::string &host, std::size_t port,
            connect_state status) {
        if (status == connect_state::ok) {
          this->createLogEntry(LogType::INFO, "Client connected to " +
              host + ":" + std::to_string(port), Verbosity::HIGHER);
        }  else if (status == connect_state::dropped) {
          this->clientConnect();
          this->createLogEntry(LogType::WARNING, "Client reconnected",
              Verbosity::NORMAL);
          // TODO(ckirchme): Handle no reconnect possible
        }
      });
  if(this->password != "") {
    this->client.auth(this->password, [this](const cpp_redis::reply& reply) {
          if (reply.is_error()) {
            this->createLogEntry(LogType::ERROR, "Authentication failed",
                Verbosity::NORMAL, true);
            exit (EXIT_FAILURE);
          }
        });
  }
}

void RedisService::subConnect() {
  this->sub.connect(this->host, this->port,
      [this](const std::string &host, std::size_t port,
             connect_state status) {
        if (status == connect_state::ok) {
          this->createLogEntry(LogType::INFO, "Subscriber connected to "
              + host + ":" + std::to_string(port), Verbosity::HIGHER);
        } else if (status == connect_state::dropped) {
          this->subConnect();
          this->createLogEntry(LogType::WARNING, "Subscriber reconnected",
              Verbosity::NORMAL);
          // TODO(ckirchme): Handle no reconnect possible
        }
      });
  if(this->password != "") {
    this->sub.auth(this->password, [this](const cpp_redis::reply& reply) {
          if (reply.is_error()) {
            this->createLogEntry(LogType::ERROR, "Authentication failed",
                Verbosity::NORMAL, true);
            exit (EXIT_FAILURE);
          }
        });
  }
}

void RedisService::createLogEntry(LogType logType, std::string message,
    Verbosity v, bool terminal) {
  if(terminal) {
    this->logger.pushToTerminal(logType, "[Redis] " + message, v);
  } else {
    this->logger.push(logType, "[Redis] " + message, v);
  }
}

void RedisService::push(std::string channel, std::string data) {
  client.publish(channel, {data}, [](cpp_redis::reply &reply) {});
  client.commit();
}

void RedisService::subscribe(std::string channel,
    subscribe_callback_t callback) {
  this->sub.subscribe(channel, callback);
  this->sub.commit();
}
