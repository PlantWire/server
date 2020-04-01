#include "../../lib/include/pwire-server-lib.h"
#include <iostream>

void sendToLoRa(const cpp_redis::reply &reply, PwireServer &server) {
  server.writeToLoRa(reply.as_string());
}

void subscriptionCallback(const std::string &channel, const std::string &msg,
                          PwireServer &server) {
  server.writeToLoRa(msg);
}

void readCallback(const boost::system::error_code &error,
                  const std::size_t bytes_transferred, PwireServer &server) {
  if (bytes_transferred > 0) {
    server.pushToFrontend(server.getInputBuffer(bytes_transferred));
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
