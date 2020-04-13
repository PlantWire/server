#include "../include/LoRaModule.h"

E32::E32(IOService & io, std::string port, int aux, int m0, int m1)
    : sP{io, port}, aux{aux}, m0{m0}, m1{m1} {
  sP.set_option(SerialPort::baud_rate(9600));
  sP.set_option(SerialPort::parity(SerialPort::parity::none));
  sP.set_option(SerialPort::character_size(SerialPort::character_size(8)));
  sP.set_option(SerialPort::stop_bits(SerialPort::stop_bits::one));
}

E32::~E32() {
  sP.close();
}
