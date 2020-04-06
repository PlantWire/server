ARCHIVE = libpwire-server
BUILD_MODE = debug

COVFLAGS += -fprofile-arcs
COVFLAGS += -ftest-coverage
COVFLAGS += -fPIC

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
	$(CXX) src/pwire-server-lib.cpp -c $(CFLAGS) -g $(CXXFLAGS) $(CPPFLAGS) $(COVFLAGS) -o build/$(ARCHIVE).o -L ../spwl/lib/build/ -lSPWL -lcpp_redis -ltacopie -pthread -Wall -Wextra -std=c++17
	ar rcs build/$(ARCHIVE).a build/$(ARCHIVE).o

build_lib_prod:
  ifeq ($(OS),Windows_NT)
    ifeq (exists, IF EXIST build (echo exists))
			md build/
    endif
  else
		mkdir -p build/
  endif
	arm-linux-gnueabihf-g++ src/pwire-server-lib.cpp -c $(CFLAGS) -O3 $(CXXFLAGS) $(CPPFLAGS) -o build/$(ARCHIVE).o -lcpp_redis -ltacopie -pthread -Wall -Wextra -std=c++17
	ar rcs build/$(ARCHIVE).a build/$(ARCHIVE).o

.PHONY: build_lib build_lib_prod

clean:
  ifeq ($(OS),Windows_NT)
		del build/$(ARCHIVE).*
		del build/$(SPWL).*
  else
		rm build/$(ARCHIVE).*
		rm build/$(SPWL).*
  endif

