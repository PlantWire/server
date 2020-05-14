#include <ostream>

#include "../include/pwire-server-lib.h"

using connect_state = cpp_redis::connect_state;

PwireServer::PwireServer(IOService &inputIo, PwireServerConfig config)
    : config{config},
    logger{
      [this](std::string message) {
        this->redis.push("pwire-frontend", message);
      }, Verbosity::HIGHEST, this->config.uuid
    },
    redis{this->logger},
    lora{inputIo, config.lora_device, config.lora_aux, config.lora_m0, config.lora_m1, this->logger} {
  this->createLogEntry(LogType::INFO, "pWire Server initialized",
      Verbosity::NORMAL);
}

void PwireServer::
registerFrontendListener(const pwire_subscribe_callback_t &callback) {
  this->createLogEntry(LogType::INFO, "Register called", Verbosity::HIGHER);
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
  this->createLogEntry(LogType::INFO, "Message sent", Verbosity::HIGHER);
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

  this->createLogEntry(LogType::INFO, "Packet glued", Verbosity::HIGHEST);
  return packet;
}

void PwireServer::readFromLoRa(read_handler_t handler) {
  readPreamble();

  auto rawHeader = readHeader();
  SPWLHeader header = SPWLPacket::getHeaderFromRaw(rawHeader);

  if (header.dataSize > 0 && header.dataSize < SPWLPacket::MAXDATASIZE) {
    auto data = readData(header.dataSize);
    auto checksum = readChecksum();
    auto packet = gluePacket(rawHeader, data, header.dataSize, checksum);

    std::pair<SPWLPacket, bool> result = SPWLPacket::encapsulatePacket(packet);
    this->createLogEntry(LogType::INFO, "Packet created", Verbosity::HIGHER);

    if (result.second) {
      this->createLogEntry(LogType::INFO, "Message received",
          Verbosity::HIGHER);
      handler(result.first, *this);
    } else {
      this->createLogEntry(LogType::WARNING, "Received invalid packet",
          Verbosity::NORMAL);
      this->readFromLoRa(handler);
    }
  } else {
    this->createLogEntry(LogType::WARNING,
        "Received invalid packet (data size)", Verbosity::NORMAL);
    this->readFromLoRa(handler);
  }
}

void PwireServer::createLogEntry(LogType logType, std::string message,
    Verbosity v) {
  this->logger.push(logType, "[Server] " + message, v);
}

PwireServerConfig
    PwireServer::parseConfig(std::string path) {
  try{
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(path, pt);
    PwireServerConfig config{};

    config.uuid = pt.get<std::string>("general.uuid");
    config.redis_host = pt.get<std::string>("redis.host");
    config.redis_port = pt.get<uint16_t>("redis.port");
    config.redis_password = pt.get<std::string>("redis.password");
    config.lora_device = pt.get<std::string>("lora.serial_device");
    config.lora_aux = pt.get<uint8_t>("lora.aux_pin");
    config.lora_m0 = pt.get<uint8_t>("lora.m0_pin");
    config.lora_m1 = pt.get<uint8_t>("lora.m1_pin");

    return config;
  } catch(boost::wrapexcept<boost::property_tree::ptree_bad_path> & e) {
    std::cout << "Configuration error: \"" + std::string{e.what()} << '"' << '\n';
    exit (EXIT_FAILURE);
  } catch(boost::wrapexcept<boost::property_tree::ini_parser::ini_parser_error> & e) {
    std::cout << "Configuration error: \"" + std::string{e.what()} << '"' << '\n';
    exit (EXIT_FAILURE);
  }
}
