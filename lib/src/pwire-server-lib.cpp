#include <optional>
#include <ostream>

#include "../include/pwire-server-lib.h"
#include "../../lib/include/SPWLPackage.h"

using SerialPort = boost::asio::serial_port;
using IOService = boost::asio::io_service;
using connect_state = cpp_redis::connect_state;

PwireServer::PwireServer(IOService &inputIo, std::string port)
    : io{inputIo}, sP{io, port} {
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
registerFrontendListener(const subscribe_callback_t &callback) {
  sub.subscribe("pwire-server",
                [this, callback](const std::string &channel,
                                 const std::string &msg) {
                  callback(channel, msg, *this);
                });
  sub.commit();
}

void PwireServer::pushToFrontend(std::string data) {
  client.publish("pwire-frontend", {data}, [](cpp_redis::reply &reply) {});
  client.commit();
}

void PwireServer::writeToLoRa(SPWLPackage data) {
  std::array<unsigned, 512> toSend{data.rawData()};
  boost::asio::write(sP, boost::asio::buffer(toSend, toSend.size()));
}

void PwireServer::readFromLoRa(read_handler_t &&handler) {
  sP.async_read_some(boost::asio::buffer(this->inputBuffer, MAX_BUFFER_LENGHT),
                     [handler, this](const boost::system::error_code &ec,
                                     std::size_t bytes_transferred) {
                       std::optional<SPWLPackage> package = SPWLPackage::
                           encapsulatePackage(
                               this->getInputBuffer(bytes_transferred));
                       handler(ec, package, *this);
                     });
}

std::string PwireServer::getInputBuffer(std::size_t bytes_transferred) {
  std::string temp(this->inputBuffer, bytes_transferred);
  return temp;
}

void PwireServer::clientConnect() {
  client.connect("127.0.0.1", 6379,
                 [this](const std::string &host, std::size_t port,
                        connect_state status) {
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
              [this](const std::string &host, std::size_t port,
                     connect_state status) {
                if (status == connect_state::dropped) {
                  std::cout << "subscriber disconnected from ";
                  std::cout << host << ":" << port << std::endl;
                  // TODO(ckirchme): Notify Client, Solve disconnect problem
                  // should_exit.notify_all();
                }
              });
}
