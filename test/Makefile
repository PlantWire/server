EXECUTABLE = pwire-server-test

CFLAGS += -g
CFLAGS += -fprofile-arcs
CFLAGS += -ftest-coverage

all:	build_test

run_test:
	cd build && ./$(EXECUTABLE)
	gcov build/pwire-server-test.cpp

build_test:
    ifeq ($(OS),Windows_NT)
        ifeq (exists, IF EXIST build (echo exists))
		md build/
        endif
    else
	mkdir -p build/
    endif
	$(CXX) -I cute/ src/$(EXECUTABLE).cpp -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o build/$(EXECUTABLE).o
	$(CXX) build/$(EXECUTABLE).o -o build/$(EXECUTABLE) -L ../lib/build/ -lpwire-server -lcpp_redis -ltacopie -pthread -lboost_system -Wall -Wextra -lgcov --coverage

clean:
    ifeq ($(OS),Windows_NT)
	del build/$(EXECUTABLE) build/$(EXECUTABLE).xml build/$(EXECUTABLE).o
    else
	rm build/$(EXECUTABLE) build/$(EXECUTABLE).xml build/$(EXECUTABLE).o
    endif
