#include <ostream>

#include "../include/pwire-server-lib.h"

using connect_state = cpp_redis::connect_state;

PwireServer::PwireServer(IOService &inputIo, std::string port, std::string uuid)
    : uuid{uuid}, lora{inputIo, port, /*A6*/1, /*A3*/3, /*A2*/2}, redis{},
    logger{this->redis, Verbosity::NORMAL, this->uuid} {
  this->createLogEntry(LogType::INFO, "pWire Server initialized",
      Verbosity::NORMAL);
}

void PwireServer::
registerFrontendListener(const pwire_subscribe_callback_t &callback) {
  this->createLogEntry(LogType::INFO, "Register called", Verbosity::HIGH);
  this->redis.subscribe("pwire-server",
      [this, callback](const std::string &channel,
                 const std::string &msg){
        callback(channel, msg, *this);
      });
}

void PwireServer::pushToFrontend(std::string data) {
  redis.push("pwire-frontend", data);
}

void PwireServer::writeToLoRa(SPWLPacket data) {
  auto temp = data.rawData();
  this->lora.send(temp, data.rawDataSize());
  this->createLogEntry(LogType::INFO, "Message sent", Verbosity::HIGH);
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
  this->createLogEntry(LogType::INFO, "Preamble read", Verbosity::HIGHEST);
}

HeaderContainer PwireServer::readHeader() {
  HeaderContainer header{};
  this->lora.receive(header, SPWLPacket::HEADERSIZE);
  // ToDo(ckirchme): Error handling (bytes read etc) Timeout?
  this->createLogEntry(LogType::INFO, "Header read", Verbosity::HIGHEST);
  return header;
}

DataContainer PwireServer::readData(uint16_t dataLength) {
  DataContainer data{};
  this->lora.receive(data, dataLength);
  createLogEntry(LogType::INFO, "Data read", Verbosity::HIGHEST);
  return data;
}

ChecksumContainer PwireServer::readChecksum() {
  ChecksumContainer checksum{};
  this->lora.receive(checksum, SPWLPacket::CHECKSUMSIZE);
  this->createLogEntry(LogType::INFO, "Checksum read", Verbosity::HIGHEST);
  return checksum;
}

PacketContainer PwireServer::gluePacket(
    HeaderContainer  header,
    DataContainer data,
    uint16_t dataLength,
    ChecksumContainer checksum) {

  PacketContainer packet{};
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

  createLogEntry(LogType::INFO, "Packet glued", Verbosity::HIGHEST);
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
    createLogEntry(LogType::INFO, "Packet created", Verbosity::HIGH);

    if (result.second) {
      handler(result.first, *this);
      createLogEntry(LogType::INFO, "Message received", Verbosity::HIGH);
      return;
    } else {
      createLogEntry(LogType::ERROR, "Packet not valid", Verbosity::NORMAL);
      // ToDo(ckirchme): Error handling
      // handler(result.first, *this);
    }
  } else {
    createLogEntry(LogType::ERROR, "Wrong packet data size", Verbosity::NORMAL);
    // ToDo(ckirchme): Error handling
    // handler(result.first, *this);
  }
}

void PwireServer::createLogEntry(Logger::LogType logType, std::string message,
    Logger::Verbosity v) {
  this->logger.push(logType, message, v);
}
