#ifndef LIB_INCLUDE_LORA_MODULE_H_
#define LIB_INCLUDE_LORA_MODULE_H_

#include <mutex>
#include <vector>
#include <string>

#include <boost/asio.hpp>
#include "./logger.h"

using SerialPort = boost::asio::serial_port;
using IOService = boost::asio::io_service;
using Verbosity = Logger::Verbosity;
using LogType = Logger::LogType;

class E32{
 private:
  enum pinState {ZERO = 0, ONE = 1};
  enum mode {NORMAL, WAKE_UP, POWER_SAVING, SLEEP};

  int aux;
  int m0;
  int m1;
  Logger &logger;

  // Default values for module configuration see E32-443T20DC documentation
  // Hex C0
  unsigned char confPrefix = 0xC0;
  unsigned char confValueAddressHigh = 0x00;
  unsigned char confValueAddressLow = 0x00;
  // HEX 1A (UART parity 8N1, baudRate 9600, Air data rate 2.4k)
  unsigned char confValueSPED = 0x1A;
  // HEX 17 (Channel 23 -> 433Mhz)
  unsigned char confValueChannel = 0x17;
  // HEX 4, Bin 00000100
  // Transparent Transmission mode, OpenDrain, 250ms wake up time, FEC On, 20dBm
  unsigned char confValueOption = 0x4;

  SerialPort sP;
  std::mutex module{};

  E32::pinState getAuxState();
  void waitForAux();

  void lockModuleWrite();

  void releaseModuleWrite();

  void setMode(E32::mode);

  void writeConfig();

  void resetModule();

  void createLogEntry(LogType logType, std::string message, Verbosity v);

 public:
  E32(IOService & io, std::string port, int aux, int m0, int m1,
      Logger &logger);

  ~E32();

  template <std::size_t R>
  void send(std::array<unsigned char, R> & toSend, size_t amount) {
    lockModuleWrite();
    boost::asio::write(sP, boost::asio::buffer(toSend, amount));
    releaseModuleWrite();
  }

  template <std::size_t R>
  size_t receive(std::array<unsigned char, R> & buffer, size_t amount) {
    size_t bytes_read =
        boost::asio::read(sP, boost::asio::buffer(buffer, amount));
    return bytes_read;
  }
};
#endif  // LIB_INCLUDE_LORA_MODULE_H_
