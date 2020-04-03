EXECUTABLE = pwire-server-test

CFLAGS += -g
COVFLAGS += -fprofile-arcs
COVFLAGS += -ftest-coverage
COVFLAGS += -fPIC

all:	build_test

run_test:
	cd build && ./$(EXECUTABLE)
	gcovr -r . -e "cute/*"

build_test:
  ifeq ($(OS),Windows_NT)
  ifeq (exists, IF EXIST build (echo exists))
		md build/
  endif
  else
		mkdir -p build/
  endif
	$(CXX) -I cute/ src/$(EXECUTABLE).cpp -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(COVFLAGS) -o build/$(EXECUTABLE).o -std=c++17
	$(CXX) build/$(EXECUTABLE).o $(COVFLAGS) -o build/$(EXECUTABLE) -L ../lib/build/ -lpwire-server -lcpp_redis -ltacopie -pthread -lboost_system -Wall -Wextra -std=c++17

clean:
  ifeq ($(OS),Windows_NT)
		del build/$(EXECUTABLE) build/$(EXECUTABLE).xml build/$(EXECUTABLE).o
  else
		rm build/$(EXECUTABLE) build/$(EXECUTABLE).xml build/$(EXECUTABLE).o
  endif
	