#include "../include/SPWLPackage.h"

SPWLPackage::SPWLPackage(uint16_t senderAddress, char channel,
    std::string data, bool last = false) {
  this->senderAddress = senderAddress;
  this->data = data;
  this->last = last;
}

std::string SPWLPackage::getData() const {
  return this->data;
}

std::array<unsigned, 512> SPWLPackage::rawData() const {
  std::array<unsigned, 512> output{};

  std::string pre{PREAMBLE};
  std::copy(pre.cbegin(), pre.cend(), output.begin());

  int i = 16;
  int pos = 7;
  while ( i > 0 ) {
    output.at(pos) = senderAddress >> i;
    i -= 8;
    pos++;
  }

  output.at(9) = channel;

  char last = 0;
  if (this->last) {
    last = 255;
  }
  output.at(10) = last;

  auto outputIter = output.begin();
  std::advance(outputIter, 10);
  std::string checksum = generateChecksum(this->data);
  outputIter = std::copy(checksum.cbegin(), checksum.cend(), outputIter);

  std::copy(this->data.cbegin(), this->data.cend(), outputIter);
  //  return "UUUUUUU" + this->senderAddress +
  //  this->channel + generateChecksum(this->data) + this->data;
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
