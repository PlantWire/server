#include "../include/redis-service.h"
RedisService::RedisService() {
  this->clientConnect();
  this->subConnect();
}

RedisService::~RedisService() {
  this->sub.disconnect();
  this->client.disconnect();
  // cpp_redis::active_logger =
  //  std::unique_ptr<cpp_redis::logger>(new cpp_redis::logger);
  // TODO(ckirchme): Enable logging see https://github.com/cpp-redis/cpp_redis/wiki/Logger
}

void RedisService::clientConnect() {
  this->client.connect("127.0.0.1", 6379,
      [this](const std::string &host, std::size_t port,
            connect_state status) {
        if (status == connect_state::dropped) {
          std::cout << "client disconnected from ";
          std::cout << host << ":" << port << std::endl;
          clientConnect();
          // TODO(ckirchme): Notify Client
          // should_exit.notify_all();
        }
      });
}

void RedisService::subConnect() {
  this->sub.connect("127.0.0.1", 6379,
      [this](const std::string &host, std::size_t port,
             connect_state status) {
        if (status == connect_state::dropped) {
          /*createLogEntry(Logger::LogType::ERROR,
                         "subscriber disconnected from " + host + ":"
                         + std::to_string(port));*/
          // TODO(ckirchme): Notify Client
          // should_exit.notify_all();
        }
      });
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
