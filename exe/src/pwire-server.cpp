#include <iostream>
#include "../../lib/include/pwire-server-lib.h"

PwireServer server{"/dev/ttyS1"};

void sendToLoRa(cpp_redis::reply const & reply) {
    server.writeToLoRa(reply.as_string());
    server.pushToFrontend(reply.as_string());
}

void subscribtionCallback(const std::string& channel, const std::string& msg) {
    if (msg.compare("lpush") == 0) {
        server.getFromFrontend(sendToLoRa);
    }
}

void readCallback(const boost::system::error_code& error,
  std::size_t bytes_transferred) {
    server.pushToFrontend("read");
    // server.pushToFrontend(result);
    // result.clear();
    // server.readFromLoRa(result, readCallback);
}

// Boost asio (asynchronous IO)
int main() {
    // PwireServer server{"/dev/ttyS1"};
    server.registerFrontendListener(subscribtionCallback);
    // server.readFromLoRa(result, readCallback);
    std::string result{};
    while (true) {
        result = server.readFromLoRa();
        server.pushToFrontend(result);
        result.clear();
    }
}
