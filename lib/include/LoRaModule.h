#ifndef LIB_INCLUDE_LORAMODULE_H_
#define LIB_INCLUDE_LORAMODULE_H_

#include <vector>
#include <string>

#include <boost/asio.hpp>

using SerialPort = boost::asio::serial_port;
using IOService = boost::asio::io_service;

class E32{
 private:
  int aux;
  int m0;
  int m1;
  SerialPort sP;
 public:
  E32(IOService & io, std::string port, int aux, int m0, int m1);

  ~E32();

  template <std::size_t R>
  void send(std::array<unsigned char, R> & toSend, size_t amount) {
    boost::asio::write(sP, boost::asio::buffer(toSend, amount));
  }

  template <std::size_t R>
  size_t receive(std::array<unsigned char, R> & buffer, size_t amount) {
    size_t bytes_read =
        boost::asio::read(sP, boost::asio::buffer(buffer, amount));
    return bytes_read;
  }
};
#endif  // LIB_INCLUDE_LORAMODULE_H_
