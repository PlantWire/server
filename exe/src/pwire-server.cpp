#include "../../lib/include/pwire-server-lib.h"
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

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
  PwireServerConfig config = PwireServer::parseConfig("config.ini");

  boost::asio::io_service io{};
  PwireServer server{io, config};
  server.registerFrontendListener(subscriptionCallback);
  server.readFromLoRa(readCallback);
  io.run();
}
