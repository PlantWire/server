#include "../include/lora-module.h"
#include <wiringPi.h>
#include <chrono>
#include <thread>

E32::E32(IOService & io, std::string port, int aux, int m0, int m1,
    Logger &logger) : sP{io, port}, aux{aux}, m0{m0}, m1{m1}, logger{logger} {
  sP.set_option(SerialPort::baud_rate(9600));
  sP.set_option(SerialPort::parity(SerialPort::parity::none));
  sP.set_option(SerialPort::character_size(SerialPort::character_size(8)));
  sP.set_option(SerialPort::stop_bits(SerialPort::stop_bits::one));

  wiringPiSetup();
  pinMode(aux, INPUT);
  pinMode(m0, OUTPUT);
  pinMode(m1, OUTPUT);

  lockModuleWrite();
  setMode(E32::mode::NORMAL);
  releaseModuleWrite();

  writeConfig();
}

E32::pinState E32::getAuxState() {
  return static_cast<E32::pinState>(digitalRead(aux));
}

void E32::waitForAux() {
  bool logged = false;
  while (getAuxState() == E32::pinState::ZERO) {
    if (!logged) {
      logged = true;
      this->createLogEntry(LogType::INFO, "Aux low",
          Verbosity::HIGHEST);
    }
  }
  this->createLogEntry(LogType::INFO, "Aux high",
      Verbosity::HIGHEST);
}

void E32::lockModuleWrite() {
  waitForAux();
  this->module.lock();
}

void E32::releaseModuleWrite() {
  this->module.unlock();
}

void E32::setMode(E32::mode mode) {
  // Only use when module locked by thread
  waitForAux();
  std::this_thread::sleep_for(std::chrono::milliseconds(2));
  switch (mode) {
    case E32::mode::NORMAL:
      digitalWrite(this->m0, E32::pinState::ZERO);
      digitalWrite(this->m1, E32::pinState::ZERO);
      this->createLogEntry(LogType::INFO, "Mode set to NORMAL",
          Verbosity::HIGHER);
      break;
    case E32::mode::WAKE_UP:
      digitalWrite(this->m0, E32::pinState::ONE);
      digitalWrite(this->m1, E32::pinState::ZERO);
      this->createLogEntry(LogType::INFO, "Mode set to WAKE_UP",
          Verbosity::HIGHER);
      break;
    case E32::mode::POWER_SAVING:
      digitalWrite(this->m0, E32::pinState::ZERO);
      digitalWrite(this->m1, E32::pinState::ONE);
      this->createLogEntry(LogType::INFO, "Mode set to POWER_SAVING",
          Verbosity::HIGHER);
      break;
    case E32::mode::SLEEP:
      digitalWrite(this->m0, E32::pinState::ONE);
      digitalWrite(this->m1, E32::pinState::ONE);
      this->createLogEntry(LogType::INFO, "Mode set to SLEEP/PROGRAM",
          Verbosity::HIGHER);
      break;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  waitForAux();
}

void E32::writeConfig() {
  lockModuleWrite();
  this->createLogEntry(LogType::INFO, "Begin writing Configuration",
      Verbosity::HIGHER);
  setMode(E32::mode::SLEEP);

  std::array<unsigned char, 6> config{};
  config[0] = this->confPrefix;
  config[1] = this->confValueAddressHigh;
  config[2] = this->confValueAddressLow;
  config[3] = this->confValueSPED;
  config[4] = this->confValueChannel;
  config[5] = this->confValueOption;

  boost::asio::write(sP, boost::asio::buffer(config));
  waitForAux();

  resetModule();

  setMode(E32::mode::NORMAL);
  releaseModuleWrite();
  this->createLogEntry(LogType::INFO, "Configuration has been written",
      Verbosity::HIGHER);
}

void E32::resetModule() {
  // Only use when module locked by thread
  this->createLogEntry(LogType::INFO, "Begin Module reset",
      Verbosity::HIGHER);
  std::array<unsigned char, 3> resetCommand{};
  resetCommand[0] = 0xC4;
  resetCommand[1] = 0xC4;
  resetCommand[2] = 0xC4;
  boost::asio::write(sP, boost::asio::buffer(resetCommand));
  waitForAux();
  this->createLogEntry(LogType::INFO, "Module has been reset",
      Verbosity::HIGHER);
}

void E32::createLogEntry(Logger::LogType logType, std::string message,
    Logger::Verbosity v) {
  this->logger.push(logType, "[LoRa] " + message, v);
}

E32::~E32() {
  sP.close();
}
