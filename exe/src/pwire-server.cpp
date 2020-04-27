#include "../../lib/include/pwire-server-lib.h"
#include <iostream>

void subscriptionCallback(const std::string &channel, const std::string &msg,
                          PwireServer &server) {
  std::vector<unsigned char> data{};
  std::copy(msg.cbegin(), msg.cend(), std::back_inserter(data));
  std::pair<SPWLPacket, bool> result = SPWLPacket::encapsulateData(data);
  if (result.second) {
    server.writeToLoRa(result.first);
  } else {
    // ToDo(ckirchme): Log data to big error
  }
}

void readCallback(SPWLPacket packet, PwireServer &server) {
  std::vector<unsigned char> tempData = packet.getData();
  std::string data{tempData.cbegin(), tempData.cend()};
  server.pushToFrontend(data);
  server.readFromLoRa(readCallback);
}

int main() {
  boost::asio::io_service io{};
  PwireServer server{io, "/dev/ttyS1", "fe2c15fc-85d2-4691-be70-f4adb326a334"};
  server.registerFrontendListener(subscriptionCallback);
  server.readFromLoRa(readCallback);
  io.run();
}
