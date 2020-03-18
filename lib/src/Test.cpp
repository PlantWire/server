#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include "../include/CppLinuxSerial/SerialPort.hpp"

using SerialPort = mn::CppLinuxSerial::SerialPort;
using Baudrate = mn::CppLinuxSerial::BaudRate;

void write(const SerialPort & cP, std::string content) {
  cP.Write(content);
}

void read(const SerialPort & cP, std::ostream & out) {
  std::string in{};
  while (true) {
      cP.Read(in);
      if (!in.empty()) {
        out << in;
        bool t = (in.compare("QUIT\n") == 0);
        out << t;
        if (in.compare("QUIT\n") == 0) {
          out << "Shutdown in Progress";
          system("shutdown -P now");
        }
      }
      in.clear();
  }
}


int main(void) {
  Baudrate constexpr bR = mn::CppLinuxSerial::BaudRate::B_9600;

  SerialPort cP{"/dev/ttyS1", bR};
  cP.SetTimeout(10);
  cP.Open();

  std::thread readThread(write, std::ref(cP), "Hello World!\n");
  std::thread writeThread(read, std::ref(cP), std::ref(std::cout));

  readThread.join();
  writeThread.join();
  cP.Close();
  return 0;
}

/*#include <cpp_redis/cpp_redis>
#include <tacopie/tacopie>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <signal.h>

std::condition_variable should_exit;

void
sigint_handler(int) {
  should_exit.notify_all();
}

int
main(void) {
  //! Enable logging
  cpp_redis::active_logger = std::unique_ptr<cpp_redis::logger>(new cpp_redis::logger);

  cpp_redis::subscriber sub;
  cpp_redis::client client;

  sub.connect("127.0.0.1", 6379, [](const std::string& host, std::size_t port, cpp_redis::connect_state status) {
    if (status == cpp_redis::connect_state::dropped) {
      std::cout << "client disconnected from " << host << ":" << port << std::endl;
      should_exit.notify_all();
    }
  });

  client.connect("127.0.0.1", 6379, [](const std::string& host, std::size_t port, cpp_redis::connect_state status){
    if (status == cpp_redis::connect_state::dropped) {
      std::cout << "client disconnected from " << host << ":" << port << std::endl;
    }
  });

  int count = 0;
  sub.subscribe("__keyspace@0__:server", [&client,count](const std::string& chan, const std::string& msg) mutable{
    std::cout << "frontend sent: " << msg << std::endl;
    client.lpush("frontend", {msg}, [](cpp_redis::reply& reply){});
    client.commit();
    count++;
  });
  sub.commit();

  signal(SIGINT, &sigint_handler);
  std::mutex mtx;
  std::unique_lock<std::mutex> l(mtx);
  should_exit.wait(l);
  return 0;
}*/
