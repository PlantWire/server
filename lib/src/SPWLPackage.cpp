#include "../include/SPWLPackage.h"

SPWLPackage::SPWLPackage(uint16_t senderAddress, char channel,
    std::string data, bool last = false) {
  this->senderAddress = senderAddress;
  this->data = data;
  this->last = last;
  this->lenght = data.size();
}

std::string SPWLPackage::getData() const {
  return this->data;
}

int SPWLPackage::rawDataSize() const {
  return this->data.size() + HEADERSIZE;
}

std::array<unsigned, 512> SPWLPackage::rawData() const {
  std::array<unsigned, 512> output{};

  for (int i = 0; i < strlen(PREAMBLE); i++) {
    output.at(i) = PREAMBLE[i];
  }

  output.at(7) = this->senderAddress >> 8;
  output.at(8) = this->senderAddress;

  output.at(9) = this->channel;

  output.at(10) = this->lenght >> 8;
  output.at(11) = this->lenght;

  char last = 0;
  if (this->last) {
    last = 255;
  }
  output.at(12) = last;

  auto outputIter = output.begin();
  std::advance(outputIter, 12);
  std::string checksum = generateChecksum(this->data);
  outputIter = std::copy(checksum.cbegin(), checksum.cend(), outputIter);

  std::copy(this->data.cbegin(), this->data.cend(), outputIter);
  return output;
}

SPWLPackage SPWLPackage::encapsulateData(std::string data) {
  SPWLPackage package{0, 0, data, true};
  return package;
}

std::optional<SPWLPackage> SPWLPackage::
  encapsulatePackage(std::string rawData) {
  if (rawData.size() > 27 && rawData.size() < 512) {
    std::string preamble = rawData.substr(7);
    if (checkPreamble(preamble)) {
      std::string checksum = rawData.substr(12, 16);
      std::string data {rawData.substr(28, rawData.size() - 27)};
      if (checkChecksum(checksum, data)) {
        uint16_t senderAddress {std::stoi(rawData.substr(8, 2))};
        char channel = rawData[10];
        bool last = rawData.substr(11, 1).compare("ÿ") == 0;
        SPWLPackage package{senderAddress, channel, data, last};
        return package;
      }
    }
  }
  return {};
}

bool SPWLPackage::checkPreamble(std::string preamble) {
  return preamble.compare("UUUUUUU") == 0;
}

bool SPWLPackage::checkChecksum(std::string checksum, std::string data) {
  return true;
}

std::string SPWLPackage::generateChecksum(std::string data) {
  return "HelloWorld!!!!!!";
}
