#ifndef LIB_INCLUDE_REDIS_SERVICE_H_
#define LIB_INCLUDE_REDIS_SERVICE_H_

#include <cpp_redis/cpp_redis>
#include <tacopie/tacopie>
#include <string>

using connect_state = cpp_redis::connect_state;
typedef std::function<void(const std::string &channel,
                           const std::string &msg)> subscribe_callback_t;

class RedisService {
 private:
  cpp_redis::subscriber sub;
  cpp_redis::client client;

  void clientConnect();
  void subConnect();
 public:
  RedisService();
  ~RedisService();
  void push(std::string channel, std::string data);
  void subscribe(std::string channel, subscribe_callback_t callback);
};


#endif  // LIB_INCLUDE_REDIS_SERVICE_H_
