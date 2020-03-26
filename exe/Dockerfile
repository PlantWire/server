FROM debian:latest

RUN apt update
RUN apt install gcc libc6-armel-cross libc6-dev-armel-cross binutils-arm-linux-gnueabi libncurses5-dev gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf git cmake make python -y
RUN git clone https://github.com/cpp-redis/cpp_redis.git
RUN cd cpp_redis && git submodule init && git submodule update
RUN mkdir cpp_redis/build
ARG CXX=/usr/bin/arm-linux-gnueabihf-g++
RUN cd cpp_redis/build && cmake .. -DCMAKE_BUILD_TYPE=Release
RUN cd cpp_redis/build && cmake -DCMAKE_INSTALL_PREFIX=/usr/arm-linux-gnueabihf/ .
RUN cd cpp_redis/build/ && make
RUN cp cpp_redis/build/lib/libcpp_redis.a /usr/arm-linux-gnueabihf/lib/
RUN cp cpp_redis/build/lib/libtacopie.a /usr/arm-linux-gnueabihf/lib/
RUN cp -r cpp_redis/includes/cpp_redis/ /usr/arm-linux-gnueabihf/include/
RUN cp -r cpp_redis/tacopie/includes/tacopie/ /usr/arm-linux-gnueabihf/include/
