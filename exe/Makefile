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
	arm-linux-gnueabihf-g++ -I include/ src/$(EXECUTABLE).cpp -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o build/$(EXECUTABLE).o
	arm-linux-gnueabihf-g++ build/$(EXECUTABLE).o -L ../lib/build/ -lpwire-server -o build/$(EXECUTABLE)

clean:
    ifeq ($(OS),Windows_NT)
	del build/$(EXECUTABLE) build/$(EXECUTABLE).o
    else
	rm build/$(EXECUTABLE) build/$(EXECUTABLE).o
    endif
