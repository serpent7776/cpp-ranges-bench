CXXFLAGS+=-std=c++23 -Wall -Wextra -pedantic -O2 -g -I lib/ranges-v3/include

all: read
	./read

read: Makefile read.o catch_amalgamated.o
	${CXX} ${CFLAGS} ${CXXFLAGS} -o read read.o catch_amalgamated.o

rangesv3:
	cd lib/ranges-v3/ && mkdir -p build && cd build && cmake ../ && make
