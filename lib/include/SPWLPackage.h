#ifndef LIB_INCLUDE_SPWLPACKAGE_H_
#define LIB_INCLUDE_SPWLPACKAGE_H_

#include <optional>
#include <string>
#include <array>
#include <cstdint>

constexpr char PREAMBLE[] = "UUUUUUU";

class SPWLPackage{
 public:
  SPWLPackage(uint16_t senderAddress, char channel,
      std::string data, bool last);

  SPWLPackage static encapsulateData(std::string data);

  std::optional<SPWLPackage> static encapsulatePackage(std::string rawData);

  bool static checkPreamble(std::string preamble);

  bool static checkChecksum(std::string checksum, std::string data);

  std::string static generateChecksum(std::string data);

  std::string getData() const;

  std::array<unsigned, 512> rawData() const;
 private:
  std::string data;
  uint16_t senderAddress;
  char channel;
  bool last;
};

#endif  // LIB_INCLUDE_SPWLPACKAGE_H_
