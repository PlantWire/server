#include "../../lib/include/pwire-server-lib.h"
#include "../../lib/include/CppLinuxSerial/SerialPort.hpp"

// Boost asio (asynchronous IO)
int main() {
    PwireServer server{"/dev/ttyS1"};
    server.pushToFrontend("Hello World");
    while (true) {}
}
