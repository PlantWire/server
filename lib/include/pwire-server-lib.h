#ifndef LIB_INCLUDE_PWIRE_SERVER_LIB_H_
#define LIB_INCLUDE_PWIRE_SERVER_LIB_H_

#include <iosfwd>
#include <mutex>
#include <string>

#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "../../spwl/lib/include/SPWL.h"
#include "./redis-service.h"
#include "./logger.h"
#include "./lora-module.h"

using SerialPort = boost::asio::serial_port;
using IOService = boost::asio::io_service;
using Verbosity = Logger::Verbosity;
using LogType = Logger::LogType;
using HeaderContainer = std::array<unsigned char, SPWLPacket::HEADERSIZE>;
using DataContainer = std::array<unsigned char, SPWLPacket::MAXDATASIZE>;
using PacketContainer = std::array<unsigned char, SPWLPacket::PACKETSIZE>;
using ChecksumContainer = std::array<unsigned char, SPWLPacket::CHECKSUMSIZE>;

class PwireServer;  // Forward declaration

typedef std::function<void(const std::string &channel,
                           const std::string &msg,
                           PwireServer &server)> pwire_subscribe_callback_t;
typedef std::function<void(SPWLPacket packet,
                           PwireServer &server)> read_handler_t;

typedef struct {
  std::string uuid;
  std::string redis_host;
  uint16_t redis_port;
  std::string redis_password;
  std::string lora_device;
  uint8_t lora_aux;
  uint8_t lora_m0;
  uint8_t lora_m1;
} PwireServerConfig;

class PwireServer {
 public:
  PwireServer(IOService &io, PwireServerConfig config);

  void
  registerFrontendListener(const pwire_subscribe_callback_t &callback);

  void pushToFrontend(std::string data);

  void writeToLoRa(SPWLPacket data);

  void readFromLoRa(read_handler_t callback);

  static PwireServerConfig parseConfig(std::string path);

 private:
  PwireServerConfig config;
  // Has to be before E32 and redis for initialization ordering
  Logger logger;
  RedisService redis;
  E32 lora;

  HeaderContainer readHeader();
  void readPreamble();
  DataContainer readData(uint16_t dataLength);
  ChecksumContainer readChecksum();
  PacketContainer gluePacket(
      HeaderContainer header,
      DataContainer data,
      uint16_t dataLength,
      ChecksumContainer checksum);

  void createLogEntry(Logger::LogType logType, std::string message,
      Logger::Verbosity v);
};


#endif  // LIB_INCLUDE_PWIRE_SERVER_LIB_H_
