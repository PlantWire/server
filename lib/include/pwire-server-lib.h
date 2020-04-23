#ifndef LIB_INCLUDE_PWIRE_SERVER_LIB_H_
#define LIB_INCLUDE_PWIRE_SERVER_LIB_H_

#include <iosfwd>
#include <mutex>
#include <string>

#include <boost/asio.hpp>

#include "../../spwl/lib/include/SPWL.h"
#include "./redis-service.h"
#include "./logger.h"
#include "./lora-module.h"

using SerialPort = boost::asio::serial_port;
using IOService = boost::asio::io_service;

class PwireServer;  // Forward declaration

typedef std::function<void(const std::string &channel,
                           const std::string &msg,
                           PwireServer &server)> pwire_subscribe_callback_t;
typedef std::function<void(SPWLPacket packet,
                           PwireServer &server)> read_handler_t;

class PwireServer {
 public:
  PwireServer(IOService &io, std::string port, std::string uuid);

  void
  registerFrontendListener(const pwire_subscribe_callback_t &callback);

  void pushToFrontend(std::string data);

  void writeToLoRa(SPWLPacket data);

  void readFromLoRa(read_handler_t &&callback);

 private:
  std::string uuid;
  E32 lora;
  RedisService redis;

  std::array<unsigned char, SPWLPacket::HEADERSIZE> readHeader();
  void readPreamble();
  std::array<unsigned char, SPWLPacket::MAXDATASIZE>
      readData(uint16_t dataLength);
  std::array<unsigned char, SPWLPacket::CHECKSUMSIZE> readChecksum();

  void createLogEntry(Logger::LogType logType, std::string message);
};


#endif  // LIB_INCLUDE_PWIRE_SERVER_LIB_H_
