#include <ostream>

#include "../include/pwire-server-lib.h"
#include "../../spwl/lib/include/SPWL.h"

using SerialPort = boost::asio::serial_port;
using IOService = boost::asio::io_service;
using connect_state = cpp_redis::connect_state;

PwireServer::PwireServer(IOService &inputIo, std::string port, std::string uuid)
    : io{inputIo}, sP{io, port}, uuid{uuid} {
  sP.set_option(SerialPort::baud_rate(9600));
  sP.set_option(SerialPort::parity(SerialPort::parity::none));
  sP.set_option(SerialPort::character_size(SerialPort::character_size(8)));
  sP.set_option(SerialPort::stop_bits(SerialPort::stop_bits::one));

  subConnect();
  clientConnect();

  createLogEntry(Logger::LogType::info, "pWire Server initialized");
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
  std::array<unsigned char, SPWLPackage::PACKETSIZE> toSend = data.rawData();
  boost::asio::write(sP, boost::asio::buffer(toSend, data.rawDataSize()));
}

void readPreamble(SerialPort & sP) {
  std::array<unsigned char, 1> inputBuffer{};
  size_t count = 0;
  auto asioBuffer = boost::asio::buffer(inputBuffer, 1);
  while (count < SPWLPackage::PREAMBLESIZE) {
    size_t bytes_read = boost::asio::read(sP, asioBuffer);
    // ToDo(ckirchme): Error handling try catch
    if (bytes_read == 1 && inputBuffer.at(0) == SPWLPackage::PREAMBLE[count]) {
      count++;
    } else if (bytes_read > 0) {
      count = 0;
    }
  }
}

std::array<unsigned char, SPWLPackage::HEADERSIZE>
    readHeaderWithoutPreamble(SerialPort & sP) {
  std::array<unsigned char, SPWLPackage::HEADERSIZE -
      SPWLPackage::PREAMBLESIZE> restOfHeader{};
  boost::asio::read(sP, boost::asio::buffer(restOfHeader));
  // ToDo(ckirchme): Error handling (bytes read etc) Timeout?

  std::array<unsigned char, SPWLPackage::HEADERSIZE> header{};
  for (size_t i = 0; i < SPWLPackage::PREAMBLESIZE; i++) {
    header.at(i) = SPWLPackage::PREAMBLE[i];
  }

  std::copy(restOfHeader.cbegin(), restOfHeader.cend(),
      header.begin() + SPWLPackage::PREAMBLESIZE);

  return header;
}

std::array<unsigned char, SPWLPackage::MAXDATASIZE + SPWLPackage::TRAILERSIZE>
    readRestOfPackage(SerialPort & sP, uint16_t dataLength) {
    std::array<unsigned char, SPWLPackage::MAXDATASIZE +
        SPWLPackage::TRAILERSIZE> buffer;
    boost::asio::read(sP, boost::asio::buffer(buffer,
        dataLength + SPWLPackage::TRAILERSIZE));
    return buffer;
}

std::array<unsigned char, SPWLPackage::PACKETSIZE> gluePacket(
    std::array<unsigned char, SPWLPackage::HEADERSIZE> header,
    std::array<unsigned char, SPWLPackage::MAXDATASIZE +
        SPWLPackage::TRAILERSIZE> restOfData) {
  std::array<unsigned char, SPWLPackage::PACKETSIZE> packet{};
  auto packetIt = std::copy(header.cbegin(), header.cend(), packet.begin());
  std::copy(restOfData.cbegin(), restOfData.cend(), packetIt);
  return packet;
}

void PwireServer::readFromLoRa(read_handler_t &&handler) {
  readPreamble(sP);
  auto header = readHeaderWithoutPreamble(sP);
  uint16_t dataLength = SPWLPackage::getLengthFromHeader(header);
  if (dataLength > 0 && dataLength < SPWLPackage::MAXDATASIZE) {
    auto restOfData = readRestOfPackage(sP, dataLength);
    auto packet = gluePacket(header, restOfData);
    std::pair<SPWLPackage, bool> result = SPWLPackage::
    encapsulatePackage(packet);
    if (result.second) {
      handler(result.first, *this);
    } else {
      // ToDo(ckirchme): Error handling
      // handler(result.first, *this);
    }
  } else {
    // ToDo(ckirchme): Error handling
    // handler(result.first, *this);
  }
}

void PwireServer::clientConnect() {
  client.connect("127.0.0.1", 6379,
                 [this](const std::string &host, std::size_t port,
                        connect_state status) {
                   if (status == connect_state::dropped) {
                     std::cout << "client disconnected from ";
                     std::cout << host << ":" << port << std::endl;
                     clientConnect();
                     // TODO(ckirchme): Notify Client
                     // should_exit.notify_all();
                   }
                 });
}

void PwireServer::subConnect() {
  sub.connect("127.0.0.1", 6379,
              [this](const std::string &host, std::size_t port,
                     connect_state status) {
                if (status == connect_state::dropped) {
                  createLogEntry(Logger::LogType::error,
                      "subscriber disconnected from " + host + ":"
                      + std::to_string(port));
                  // TODO(ckirchme): Notify Client
                  // should_exit.notify_all();
                }
              });
}

void PwireServer::createLogEntry(Logger::LogType logType, std::string message) {
  std::string logEntry = Logger::generateLogEntry(logType, message, this->uuid);
  pushToFrontend(logEntry);
}
