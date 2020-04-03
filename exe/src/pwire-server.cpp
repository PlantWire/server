#include "../../lib/include/pwire-server-lib.h"
#include <iostream>
#include <optional>

/*void sendToLoRa(const cpp_redis::reply &reply, PwireServer &server) {
  server.writeToLoRa(reply.as_string());
}*/

void subscriptionCallback(const std::string &channel, const std::string &msg,
                          PwireServer &server) {
  server.writeToLoRa(SPWLPackage::encapsulateData(msg));
}

void readCallback(const boost::system::error_code &error,
                  const std::optional <SPWLPackage> package,
                  PwireServer &server) {
  if (package.has_value()) {
    server.pushToFrontend(package.value().getData());
  }
  server.readFromLoRa(readCallback);
}

int main() {
  boost::asio::io_service io{};
  PwireServer server{io, "/dev/ttyS1"};
  server.registerFrontendListener(subscriptionCallback);
  server.readFromLoRa(readCallback);
  io.run();
}
