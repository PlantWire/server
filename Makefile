run_test:
	cd test/ && make run_test

build_test: build_lib
	cd test/ && make build_test

clean_test:
	cd test/ && make clean

build_lib:
	cd lib/ && make build_lib

lint:
	cpplint --filter=-legal/copyright --recursive --exclude=test/cute --exclude=.metadata .
