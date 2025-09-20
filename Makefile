.PHONY: all clean

all:
	mkdir -p build
	cd build && cmake .. && make -j$(shell nproc)

clean:
	rm -rf build

run: all
	./build/bin/finbroker-nng
