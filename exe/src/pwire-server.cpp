#include "../../lib/include/pwire-server-lib.h"
#include <iostream>

/*void sendToLoRa(const cpp_redis::reply &reply, PwireServer &server) {
  server.writeToLoRa(reply.as_string());
}*/

void subscriptionCallback(const std::string &channel, const std::string &msg,
                          PwireServer &server) {
  std::pair<SPWLPackage, bool> result = SPWLPackage::encapsulateData(msg);
  if (result.second) {
    server.writeToLoRa(result.first);
  } else {
    // ToDo(ckirchme): Log data to big error
  }
}

void readCallback(SPWLPackage package, PwireServer &server) {
  server.pushToFrontend(package.getData());
  server.readFromLoRa(readCallback);
}

int main() {
  boost::asio::io_service io{};
  PwireServer server{io, "/dev/ttyS1"};
  server.registerFrontendListener(subscriptionCallback);
  server.readFromLoRa(readCallback);
  io.run();
}
