#include "../include/pwire-server-lib.h"

#include <ostream>

using SerialPort = boost::asio::serial_port;
using IOService = boost::asio::io_service;
using connect_state = cpp_redis::connect_state;

PwireServer::PwireServer(IOService & inputIo, std::string port) : io{inputIo}, sP{io, port} {
  sP.set_option(SerialPort::baud_rate(9600));
  sP.set_option(SerialPort::parity(SerialPort::parity::none));
  sP.set_option(SerialPort::character_size(SerialPort::character_size(8)));
  sP.set_option(SerialPort::stop_bits(SerialPort::stop_bits::one));

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
registerFrontendListener(const subscribe_callback_t & callback) {
  sub.subscribe("__keyspace@0__:pwire-server", [this, callback](const std::string & channel, const std::string & msg){
    callback(channel, msg, *this);
  });
  sub.commit();
}

void PwireServer::pushToFrontend(std::string data) {
  client.lpush("pwire-frontend", {data}, [](cpp_redis::reply& reply){});
  client.commit();
}

void PwireServer::getFromFrontend(const reply_callback_t &reply_callback) {
  client.lpop("pwire-server", [this, reply_callback](cpp_redis::reply & reply){
    reply_callback(reply, *this);
  });
  client.commit();
}

void PwireServer::writeToLoRa(std::string data) {
  boost::asio::write(sP, boost::asio::buffer(data, sizeof(data)));
}

void PwireServer::readFromLoRa(read_handler_t && handler){
  sP.async_read_some(boost::asio::buffer(this->inputBuffer, max_buffer_length), [handler, this](const boost::system::error_code& ec,
    std::size_t bytes_transferred){
      handler(ec, bytes_transferred, *this);
  });
}

std::string PwireServer::getInputBuffer(std::size_t bytes_transferred){
  char temp[bytes_transferred];
  strncpy(temp, this->inputBuffer, bytes_transferred);
  return temp;
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
