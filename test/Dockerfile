FROM debian:latest

RUN apt update
RUN apt install gcc libasio-dev libboost-system-dev python-pip git cmake make -y
RUN pip install gcovr
RUN git clone https://github.com/cpp-redis/cpp_redis.git
RUN cd cpp_redis && git submodule init && git submodule update
RUN mkdir cpp_redis/build
RUN cd cpp_redis/build && cmake .. -DCMAKE_BUILD_TYPE=Release
RUN cd cpp_redis/build && make
RUN cd cpp_redis/build && make install
