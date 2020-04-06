run_test:
	cd test/ && make run_test

build_test: build_lib
	cd test/ && make build_test

clean_test:
	cd test/ && make clean

build_lib: build_spwl
	cd lib/ && make build_lib

build_lib_prod: build_spwl_prod
	cd lib/ && make build_lib_prod

build_prod:	build_lib_prod
	cd exe/ && make build_prod

lint:
	cpplint --filter=-legal/copyright,-build/c++11,-runtime/references --recursive --exclude=lib/include/CppLinuxSerial --exclude=lib/src/CppLinuxSerial --exclude=test/cute --exclude=spwl/ --exclude=.metadata .

#Initialise submodule
build_spwl_prod:
	cd spwl/ && make build_lib_prod

build_spwl:
	cd spwl/ && make build_lib