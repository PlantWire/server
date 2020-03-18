ARCHIVE = libpwire-server
BUILD_MODE = debug

#ifeq ($(BUILD_MODE),debug)
#	CFLAGS += -g
#else ifeq ($(BUILD_MODE),run)
#	CFLAGS += -O3
#else
#	$(error Build mode $(BUILD_MODE) not supported by this Makefile)
#endif

all:	build_lib

build_lib:
    ifeq ($(OS),Windows_NT)
        ifeq (exists, IF EXIST build (echo exists))
		md build/
        endif
    else
	mkdir -p build/
    endif
	$(CXX) src/pwire-server-lib.cpp -c $(CFLAGS) -g $(CXXFLAGS) $(CPPFLAGS) -o build/$(ARCHIVE).o -lcpp_redis -ltacopie -pthread -Wall -Wextra
	$(CXX) src/CppLinuxSerial/SerialPort.cpp -c $(CFLAGS) -g $(CXXFLAGS) $(CPPFLAGS) -o build/SerialPort.o -Wall -Wextra
	ar rcs build/$(ARCHIVE).a build/$(ARCHIVE).o build/SerialPort.o

build_lib_prod:
    ifeq ($(OS),Windows_NT)
        ifeq (exists, IF EXIST build (echo exists))
		md build/
        endif
    else
	mkdir -p build/
    endif
	arm-linux-gnueabihf-g++ src/pwire-server-lib.cpp -c $(CFLAGS) -O3 $(CXXFLAGS) $(CPPFLAGS) -o build/$(ARCHIVE).o -lcpp_redis -ltacopie -pthread -Wall -Wextra
	arm-linux-gnueabihf-g++ src/CppLinuxSerial/SerialPort.cpp -c $(CFLAGS) -g $(CXXFLAGS) $(CPPFLAGS) -o build/SerialPort.o -Wall -Wextra
	ar rcs build/$(ARCHIVE).a build/$(ARCHIVE).o build/SerialPort.o

clean:
    ifeq ($(OS),Windows_NT)
	del build/$(ARCHIVE).*
    else
	rm build/$(ARCHIVE).*
    endif

