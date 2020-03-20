#include "../include/pwire-server-lib.h"

#include <ostream>

// using SerialPort = mn::CppLinuxSerial::SerialPort;
using SerialPort = boost::asio::serial_port;
using connect_state = cpp_redis::connect_state;

PwireServer::PwireServer(std::string port):sP{io, port} {
  sP.set_option(SerialPort::baud_rate(9600));
  sP.set_option(SerialPort::parity(SerialPort::parity::none));
  sP.set_option(SerialPort::character_size(SerialPort::character_size(8)));
  sP.set_option(SerialPort::stop_bits(SerialPort::stop_bits::one));
  io.run();

  subConnect();
  clientConnect();
  // cpp_redis::active_logger =
  //  std::unique_ptr<cpp_redis::logger>(new cpp_redis::logger);
  // TODO(ckirchme): Enable logging see https://github.com/cpp-redis/cpp_redis/wiki/Logger
}

PwireServer::~PwireServer() {
  sP.close();
  sub.disconnect();
  client.disconnect();
}

void PwireServer::
registerFrontendListener(const subscribe_callback_t &callback) {
  sub.subscribe("__keyspace@0__:pwire-server", callback);
  sub.commit();
}

void PwireServer::pushToFrontend(std::string data) {
  client.lpush("pwire-frontend", {data}, [](cpp_redis::reply& reply){});
  client.commit();
}

void PwireServer::getFromFrontend(const reply_callback_t &reply_callback) {
  client.lpop("pwire-server", reply_callback);
  client.commit();
}

void PwireServer::writeToLoRa(std::string data) {
  sP.write_some(boost::asio::buffer(& data, data.size()));
}

/*void PwireServer::readFromLoRa(std::string & buffer, ReadHandler && handler){
  // sP.async_read_some(boost::asio::buffer(&buffer, 8), handler);
}*/

std::string PwireServer::readFromLoRa() {
  // Reading data char by char, code is optimized for simplicity, not speed
  char c;
  std::string result;
  for (;;) {
      boost::asio::read(sP, boost::asio::buffer(&c, 1));
      switch (c) {
          case '\r':
              break;
          case '\n':
              return result;
          default:
              result+=c;
      }
  }
}

void PwireServer::clientConnect() {
  client.connect("127.0.0.1", 6379,
  [this](const std::string& host, std::size_t port, connect_state status) {
    if (status == connect_state::dropped) {
      std::cout << "client disconnected from ";
      std::cout << host << ":" << port << std::endl;
      clientConnect();
      // TODO(ckirchme): Notify Client, Solve disconnect problem
      // should_exit.notify_all();
    }
  });
}

void PwireServer::subConnect() {
    sub.connect("127.0.0.1", 6379,
  [this](const std::string& host, std::size_t port, connect_state status) {
    if (status == connect_state::dropped) {
      std::cout << "subscriber disconnected from ";
      std::cout << host << ":" << port << std::endl;
      // TODO(ckirchme): Notify Client, Solve disconnect problem
      // should_exit.notify_all();
    }
  });
}
