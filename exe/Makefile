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
	arm-linux-gnueabihf-g++ src/$(EXECUTABLE).cpp -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o build/$(EXECUTABLE).o -Wall -Wextra -std=c++14 --static
	arm-linux-gnueabihf-g++ build/$(EXECUTABLE).o -o build/$(EXECUTABLE) -L ../lib/build/ -lpwire-server -lcpp_redis -ltacopie -lpthread -lboost_system -lc -Wall -Wextra -std=c++14 --static

build:
  ifeq ($(OS),Windows_NT)
    ifeq (exists, IF EXIST build (echo exists))
      md build/
    endif
  else
		mkdir -p build/
  endif
	g++ src/$(EXECUTABLE).cpp -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o build/$(EXECUTABLE).o -Wall -Wextra -std=c++14
	g++ build/$(EXECUTABLE).o -o build/$(EXECUTABLE) -L ../lib/build/ -lpwire-server -lcpp_redis -ltacopie -lpthread -lboost_system -Wall -Wextra

.PHONY: build

clean:
  ifeq ($(OS),Windows_NT)
		del build/$(EXECUTABLE) build/$(EXECUTABLE).o
  else
		rm build/$(EXECUTABLE) build/$(EXECUTABLE).o
  endif
