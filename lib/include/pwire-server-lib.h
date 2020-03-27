#ifndef LIB_INCLUDE_PWIRE_SERVER_LIB_H_
#define LIB_INCLUDE_PWIRE_SERVER_LIB_H_

#include <cpp_redis/cpp_redis>
#include <tacopie/tacopie>

#include <iosfwd>
#include <mutex>
#include <string>

// #include "CppLinuxSerial/SerialPort.hpp"
#include <boost/asio.hpp>

using SerialPort = boost::asio::serial_port;
using IOService = boost::asio::io_service;

constexpr int MAX_BUFFER_LENGHT = 1024;

class PwireServer;  // Forward declaration

typedef std::function<void(const cpp_redis::reply &reply,
                           PwireServer &server)> reply_callback_t;
typedef std::function<void(const std::string &channel,
                           const std::string &msg,
                           PwireServer &server)> subscribe_callback_t;
typedef std::function<void(const boost::system::error_code &ec,
                           std::size_t bytes_transferred,
                           PwireServer &server)> read_handler_t;

class PwireServer {
public:
  PwireServer(IOService &io, std::string port);

  ~PwireServer();

  void
  registerFrontendListener(const subscribe_callback_t &subscribe_callback);

  void pushToFrontend(std::string data);

  void getFromFrontend(const reply_callback_t &reply_callback);

  void writeToLoRa(std::string data);

  void readFromLoRa(read_handler_t &&callback);

  std::string getInputBuffer(std::size_t bytes_transferred);
  // std::string readFromLoRa();
private:
  IOService &io;
  SerialPort sP;
  std::mutex serialConn{};
  char inputBuffer[MAX_BUFFER_LENGHT];
  cpp_redis::subscriber sub;
  cpp_redis::client client;

  void clientConnect();

  void subConnect();
};


#endif  // LIB_INCLUDE_PWIRE_SERVER_LIB_H_
