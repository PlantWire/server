#include <ostream>

#include "../include/pwire-server-lib.h"
#include "../include/LoRaModule.h"
#include "../../spwl/lib/include/SPWL.h"

using SerialPort = boost::asio::serial_port;
using IOService = boost::asio::io_service;
using connect_state = cpp_redis::connect_state;

PwireServer::PwireServer(IOService &inputIo, std::string port, std::string uuid)
    : uuid{uuid}, lora{inputIo, port, /*A6*/1, /*A3*/3, /*A2*/2} {
  subConnect();
  clientConnect();

  createLogEntry(Logger::LogType::info, "pWire Server initialized");
  // cpp_redis::active_logger =
  //  std::unique_ptr<cpp_redis::logger>(new cpp_redis::logger);
  // TODO(ckirchme): Enable logging see https://github.com/cpp-redis/cpp_redis/wiki/Logger
}

PwireServer::~PwireServer() {
  sub.disconnect();
  client.disconnect();
}

void PwireServer::
registerFrontendListener(const subscribe_callback_t &callback) {
  createLogEntry(Logger::LogType::info, "Register called");
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

void PwireServer::writeToLoRa(SPWLPacket data) {
  auto temp = data.rawData();
  this->lora.send(temp, data.rawDataSize());
  createLogEntry(Logger::LogType::info, "Message sent");
}

void PwireServer::readPreamble() {
  std::array<unsigned char, 1> inputBuffer{};
  size_t count = 0;
  while (count < SPWLPacket::PREAMBLESIZE) {
    size_t bytes_read = this->lora.receive(inputBuffer, 1);
    // ToDo(ckirchme): Error handling try catch
    if (bytes_read == 1 && inputBuffer.at(0) == SPWLPacket::PREAMBLE[count]) {
      count++;
    } else if (bytes_read > 0) {
      count = 0;
    }
  }
}

std::array<unsigned char, SPWLPacket::HEADERSIZE> PwireServer::readHeader() {
  std::array<unsigned char, SPWLPacket::HEADERSIZE> header{};
  this->lora.receive(header, SPWLPacket::HEADERSIZE);
  // ToDo(ckirchme): Error handling (bytes read etc) Timeout?
  return header;
}

std::array<unsigned char, SPWLPacket::MAXDATASIZE>
    PwireServer::readData(uint16_t dataLength) {
  std::array<unsigned char, SPWLPacket::MAXDATASIZE> data{};
  this->lora.receive(data, dataLength);
  return data;
}

std::array<unsigned char, SPWLPacket::CHECKSUMSIZE>
    PwireServer::readChecksum() {
  std::array<unsigned char, SPWLPacket::CHECKSUMSIZE> checksum{};
  this->lora.receive(checksum, SPWLPacket::CHECKSUMSIZE);
  return checksum;
}

std::array<unsigned char, SPWLPacket::PACKETSIZE> gluePacket(
    std::array<unsigned char, SPWLPacket::HEADERSIZE>  header,
    std::array<unsigned char, SPWLPacket::MAXDATASIZE> data,
    uint16_t dataLength,
    std::array<unsigned char, SPWLPacket::CHECKSUMSIZE> checksum) {

  std::array<unsigned char, SPWLPacket::PACKETSIZE> packet{};
  size_t count = 0;
  std::generate(packet.begin(),
      packet.begin() + SPWLPacket::PREAMBLESIZE, [count]() mutable {
    auto res = SPWLPacket::PREAMBLE[count];
    count++;
    return res;
  });

  auto packetIt = std::copy(header.cbegin(), header.cend(),
      packet.begin() + SPWLPacket::PREAMBLESIZE);
  packetIt = std::copy(data.cbegin(), data.cbegin() + dataLength, packetIt);
  packetIt = std::copy(checksum.cbegin(), checksum.cend(), packetIt);

  *packetIt = SPWLPacket::TRAILER;

  return packet;
}

void PwireServer::readFromLoRa(read_handler_t &&handler) {
  readPreamble();
  auto header = readHeader();
  uint16_t dataLength = SPWLPacket::getLengthFromHeader(header);

  if (dataLength > 0 && dataLength < SPWLPacket::MAXDATASIZE) {
    auto data = readData(dataLength);
    auto checksum = readChecksum();
    auto packet = gluePacket(header, data, dataLength, checksum);

    std::pair<SPWLPacket, bool> result = SPWLPacket::
    encapsulatePacket(packet);
    if (result.second) {
      handler(result.first, *this);
      createLogEntry(Logger::LogType::info, "Message received");
      return;
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
