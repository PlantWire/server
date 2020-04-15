#ifndef LIB_INCLUDE_LORAMODULE_H_
#define LIB_INCLUDE_LORAMODULE_H_

#include <mutex>
#include <vector>
#include <string>

#include <boost/asio.hpp>

using SerialPort = boost::asio::serial_port;
using IOService = boost::asio::io_service;

class E32{
 private:
  enum pinState {ZERO = 0, ONE = 1};
  enum mode {NORMAL, WAKE_UP, POWER_SAVING, SLEEP};

  int aux;
  int m0;
  int m1;

  // Default values for module configuration see E32-443T20DC documentation
  // Hex C0
  unsigned char confPrefix = 192;
  unsigned char confValueAddressHigh = 0;
  unsigned char confValueAddressLow = 0;
  // HEX 1A (UART parity 8N1, baudRate 9600, Air data rate 2.4k)
  unsigned char confValueSPED = 26;
  // HEX 17 (Channel 23 -> 433Mhz)
  unsigned char confValueChannel = 23;
  // HEX 44, Bin 00000100
  // Transparent Transmission mode, OpenDrain, 250ms wake up time, FEC On, 20dBm
  unsigned char confValueOption = 4;

  SerialPort sP;
  std::mutex module{};

  E32::pinState getAuxState();
  void waitForAux();

  void lockModule();

  void releaseModule();

  void setMode(E32::mode);

  void writeConfig();

 public:
  E32(IOService & io, std::string port, int aux, int m0, int m1);

  ~E32();

  template <std::size_t R>
  void send(std::array<unsigned char, R> & toSend, size_t amount) {
    lockModule();
    boost::asio::write(sP, boost::asio::buffer(toSend, amount));
    releaseModule();
  }

  template <std::size_t R>
  size_t receive(std::array<unsigned char, R> & buffer, size_t amount) {
    lockModule();
    size_t bytes_read =
        boost::asio::read(sP, boost::asio::buffer(buffer, amount));
    releaseModule();
    return bytes_read;
  }
};
#endif  // LIB_INCLUDE_LORAMODULE_H_
