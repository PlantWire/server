#include <iostream>
#include "../../lib/include/pwire-server-lib.h"

void sendToLoRa(cpp_redis::reply const & reply, PwireServer & server) {
    server.writeToLoRa(reply.as_string());
}

void subscribtionCallback(const std::string& channel, const std::string& msg, PwireServer & server) {
    if (msg.compare("lpush") == 0) {
        server.getFromFrontend(sendToLoRa);
    }
}

void readCallback(const boost::system::error_code& error,
  std::size_t bytes_transferred, PwireServer & server) {
    // server.pushToFrontend("read");
    if(bytes_transferred > 0){
        //server.pushToFrontend(server.getInputBuffer(bytes_transferred));
    }
    server.readFromLoRa(readCallback);
}

// Boost asio (asynchronous IO)
int main() {
    boost::asio::io_service io{};
    PwireServer server{io, "/dev/ttyS1"};
    server.registerFrontendListener(subscribtionCallback);
    server.readFromLoRa(readCallback);
    io.run();
}
