#ifndef LIB_INCLUDE_PWIRE_SERVER_LIB_H_
#define LIB_INCLUDE_PWIRE_SERVER_LIB_H_

#include <cpp_redis/cpp_redis>
#include <tacopie/tacopie>

#include <iosfwd>
#include <mutex>
#include <string>
#include <optional>

// #include "CppLinuxSerial/SerialPort.hpp"
#include <boost/asio.hpp>

typedef std::function< void(cpp_redis::reply &)>
  reply_callback_t;
typedef std::function< void(const std::string &, const std::string &)>
  subscribe_callback_t;

// using SerialPort = mn::CppLinuxSerial::SerialPort;
using SerialPort = boost::asio::serial_port;

class PwireServer {
 public:
  explicit PwireServer(std::string port);
  ~PwireServer();
  void registerFrontendListener(const subscribe_callback_t &subscribe_callback);
  void pushToFrontend(std::string);
  void getFromFrontend(const reply_callback_t &reply_callback);
  void writeToLoRa(std::string);
 private:
  boost::asio::io_service io;
  const SerialPort sP;
  std::mutex serialConn{};
  cpp_redis::subscriber sub;
  cpp_redis::client client;
};


#endif  // LIB_INCLUDE_PWIRE_SERVER_LIB_H_
