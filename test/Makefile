EXECUTABLE = pwire-server-test
BUILD_MODE = debug

ifeq ($(BUILD_MODE),debug)
	CFLAGS += -g
else ifeq ($(BUILD_MODE),run)
	CFLAGS += -O3
else
	$(error Build mode $(BUILD_MODE) not supported by this Makefile)
endif

all:	build_test

run_test:
	cd build && ./$(EXECUTABLE)

build_test:
    ifeq ($(OS),Windows_NT)
        ifeq (exists, IF EXIST build (echo exists))
		md build/
        endif
    else
	mkdir -p build/
    endif
	$(CXX) -I cute/ src/$(EXECUTABLE).cpp -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o build/$(EXECUTABLE).o
	$(CXX) build/$(EXECUTABLE).o -L ../lib/build/ -lpwire-server -o build/$(EXECUTABLE)

clean:
    ifeq ($(OS),Windows_NT)
	del build/$(EXECUTABLE) build/$(EXECUTABLE).xml build/$(EXECUTABLE).o
    else
	rm build/$(EXECUTABLE) build/$(EXECUTABLE).xml build/$(EXECUTABLE).o
    endif
