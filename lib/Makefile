ARCHIVE = libpwire-server
BUILD_MODE = debug

ifeq ($(BUILD_MODE),debug)
	CFLAGS += -g
else ifeq ($(BUILD_MODE),run)
	CFLAGS += -O3
else
	$(error Build mode $(BUILD_MODE) not supported by this Makefile)
endif

all:	build_lib

build_lib:
    ifeq ($(OS),Windows_NT)
        ifeq (exists, IF EXIST build (echo exists))
		md build/
        endif
    else
	mkdir -p build/
    endif
	$(CXX) -Iinclude/ src/pwire-server-lib.cpp -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o build/$(ARCHIVE).o
	ar rcs build/$(ARCHIVE).a build/$(ARCHIVE).o

clean:
    ifeq ($(OS),Windows_NT)
	del build/$(ARCHIVE).*
    else
	rm build/$(ARCHIVE).*
    endif

