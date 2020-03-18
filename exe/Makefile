EXECUTABLE = pwire-server
#Only production supported (no BUILD_MODE)
CFLAGS += -O3

all:	build_prod

build_prod:
    ifeq ($(OS),Windows_NT)
        ifeq (exists, IF EXIST build (echo exists))
		md build/
        endif
    else
	mkdir -p build/
    endif
	arm-linux-gnueabihf-g++ src/$(EXECUTABLE).cpp -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o build/$(EXECUTABLE).o -Wall -Wextra
	arm-linux-gnueabihf-g++ build/$(EXECUTABLE).o -o build/$(EXECUTABLE) -L ../lib/build/ -lpwire-server -lcpp_redis -ltacopie -pthread -lboost_system -Wall -Wextra

build:
    ifeq ($(OS),Windows_NT)
        ifeq (exists, IF EXIST build (echo exists))
        md build/
        endif
    else
	mkdir -p build/
    endif
	g++ src/$(EXECUTABLE).cpp -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o build/$(EXECUTABLE).o -Wall -Wextra
	g++ build/$(EXECUTABLE).o -o build/$(EXECUTABLE) -L ../lib/build/ -lpwire-server -lcpp_redis -ltacopie -pthread -lboost_system -Wall -Wextra

.PHONY: build

clean:
    ifeq ($(OS),Windows_NT)
	del build/$(EXECUTABLE) build/$(EXECUTABLE).o
    else
	rm build/$(EXECUTABLE) build/$(EXECUTABLE).o
    endif
