FROM debian:latest

RUN apt update
RUN apt install gcc g++ libc6-armel-cross libc6-dev-armel-cross binutils-arm-linux-gnueabi libncurses5-dev gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf git cmake make python wget -y
#Redis
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
#Boost
RUN wget https://dl.bintray.com/boostorg/release/1.72.0/source/boost_1_72_0.tar.gz
RUN tar -zxf boost_1_72_0.tar.gz
ARG CXX=''
ARG PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
RUN cd boost_1_72_0/ && ./bootstrap.sh --prefix=/usr/arm-linux-gnueabihf/ --with-libraries=system
RUN cd boost_1_72_0/ && sed -i 's/using gcc ;/using gcc : arm : arm-linux-gnueabihf-g++ ;/g' project-config.jam
RUN cd boost_1_72_0/ && ./b2 link=static
RUN cd boost_1_72_0/ && ./b2 headers
RUN cd boost_1_72_0/ && cp stage/lib/libboost_system.a /usr/arm-linux-gnueabihf/lib/
RUN cd boost_1_72_0/ && cp -r boost /usr/arm-linux-gnueabihf/include/
RUN git clone https://github.com/PlantWire/WiringNP.git
RUN cd WiringNP/wiringPi/ && sed -i 's/CFLAGS\s=/CFLAGS  = -fpermissive /g' Makefile
RUN cd WiringNP/wiringPi/ && make static PREFIX=/arm-linux-gnueabihf CC=/usr/bin/arm-linux-gnueabihf-g++
RUN cd WiringNP/wiringPi/ && make install-static PREFIX=/arm-linux-gnueabihf CC=/usr/bin/arm-linux-gnueabihf-g++
RUN rm -r cpp_redis
RUN rm -r boost_1_72_0
RUN rm -r WiringNP