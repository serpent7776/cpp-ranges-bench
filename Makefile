CXXFLAGS+=-std=c++23 -Wall -Wextra -pedantic -O2 -g -I lib/ranges-v3/include

all: read
	./read

read.o: lib/flux.hpp rangesv3

read: Makefile read.o catch_amalgamated.o
	${CXX} ${CFLAGS} ${CXXFLAGS} -o read read.o catch_amalgamated.o

lib/ranges-v3/build/:
	cd lib/ranges-v3/ && mkdir -p build && cd build && cmake ../

rangesv3: lib/ranges-v3/build/
	${MAKE} -C lib/ranges-v3/build
