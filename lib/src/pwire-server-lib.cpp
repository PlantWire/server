#include <ostream>

#include "../include/pwire-server-lib.h"

using SerialPort = boost::asio::serial_port;
using IOService = boost::asio::io_service;
using connect_state = cpp_redis::connect_state;

PwireServer::PwireServer(IOService &inputIo, std::string port, std::string uuid)
    : uuid{uuid}, lora{inputIo, port, /*A6*/1, /*A3*/3, /*A2*/2}, redis{} {

  createLogEntry(Logger::LogType::INFO, "pWire Server initialized");
}

void PwireServer::
registerFrontendListener(const pwire_subscribe_callback_t &callback) {
  createLogEntry(Logger::LogType::INFO, "Register called");
  this->redis.subscribe("pwire-server",
      [this, callback](const std::string &channel,
                 const std::string &msg){
        callback(channel, msg, *this);
      });

  /*sub.subscribe("pwire-server",
                [this, callback](const std::string &channel,
                                 const std::string &msg) {
                  callback(channel, msg, *this);
                });
  sub.commit();*/
}

void PwireServer::pushToFrontend(std::string data) {
  /*client.publish("pwire-frontend", {data}, [](cpp_redis::reply &reply) {});
  client.commit();*/
  redis.push("pwire-frontend", data);
}

void PwireServer::writeToLoRa(SPWLPacket data) {
  auto temp = data.rawData();
  this->lora.send(temp, data.rawDataSize());
  createLogEntry(Logger::LogType::INFO, "Message sent");
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
      createLogEntry(Logger::LogType::INFO, "Message received");
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

void PwireServer::createLogEntry(Logger::LogType logType, std::string message) {
  std::string logEntry = Logger::generateLogEntry(logType, message, this->uuid);
  pushToFrontend(logEntry);
}
