#include "../include/pwire-server-lib.h"

#include <ostream>

// using SerialPort = mn::CppLinuxSerial::SerialPort;
using SerialPort = boost::asio::serial_port;
using connect_state = cpp_redis::connect_state;

PwireServer::PwireServer(std::string port):sP{io, port} {
  // TODO(ckirchme): Make more flexible with arguments
  /*mn::CppLinuxSerial::BaudRate constexpr bR =
    mn::CppLinuxSerial::BaudRate::B_9600;
  SerialPort temp{"/dev/ttyS1", bR};
  sP = temp;
  sP.SetTimeout(10);
  sP.Open();*/

  sub.connect("127.0.0.1", 6379,
  [](const std::string& host, std::size_t port, connect_state status) {
    if (status == connect_state::dropped) {
      std::cout << "client disconnected from ";
      std::cout << host << ":" << port << std::endl;
      // TODO(ckirchme): Notify Client
      // should_exit.notify_all();
    }
  });

  client.connect("127.0.0.1", 6379,
  [](const std::string& host, std::size_t port, connect_state status){
    if (status == connect_state::dropped) {
      std::cout << "client disconnected from ";
      std::cout << host << ":" << port << std::endl;
      // TODO(ckirchme): Notify Client
      // should_exit.notify_all();
    }
  });

  // cpp_redis::active_logger =
  //  std::unique_ptr<cpp_redis::logger>(new cpp_redis::logger);
  // TODO(ckirchme): Enable logging see https://github.com/cpp-redis/cpp_redis/wiki/Logger
}

PwireServer::~PwireServer() {
  // sP.Close();
  sub.disconnect();
  client.disconnect();
}

void PwireServer::
registerFrontendListener(const subscribe_callback_t &callback) {
  sub.subscribe("__keyspace@0__:pwire-server", callback);
  sub.commit();
}

void PwireServer::pushToFrontend(std::string data) {
  client.lpush("pwire-frontend", {data}, [](cpp_redis::reply& reply){});
  client.commit();
}

void PwireServer::getFromFrontend(const reply_callback_t &reply_callback) {
  client.lpop("pwire-server", reply_callback);
  client.commit();
}
