#ifndef LIB_INCLUDE_PWIRE_SERVER_LIB_H_
#define LIB_INCLUDE_PWIRE_SERVER_LIB_H_

#include <cpp_redis/cpp_redis>
#include <tacopie/tacopie>

#include <iosfwd>
#include <mutex>
#include <string>

#include <boost/asio.hpp>

#include "../../spwl/lib/include/SPWL.h"
#include "./logger.h"

using SerialPort = boost::asio::serial_port;
using IOService = boost::asio::io_service;

class PwireServer;  // Forward declaration

typedef std::function<void(const cpp_redis::reply &reply,
                           PwireServer &server)> reply_callback_t;
typedef std::function<void(const std::string &channel,
                           const std::string &msg,
                           PwireServer &server)> subscribe_callback_t;
typedef std::function<void(SPWLPackage package,
                           PwireServer &server)> read_handler_t;

class PwireServer {
 public:
  PwireServer(IOService &io, std::string port, std::string uuid);

  ~PwireServer();

  void
  registerFrontendListener(const subscribe_callback_t &subscribe_callback);

  void pushToFrontend(std::string data);

  void writeToLoRa(SPWLPackage data);

  void readFromLoRa(read_handler_t &&callback);

 private:
  std::string uuid;
  IOService &io;
  SerialPort sP;
  cpp_redis::subscriber sub;
  cpp_redis::client client;

  void clientConnect();

  void subConnect();

  void createLogEntry(Logger::LogType logType, std::string message);
};


#endif  // LIB_INCLUDE_PWIRE_SERVER_LIB_H_
