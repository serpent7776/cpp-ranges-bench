CXXFLAGS+=-std=c++23 -Wall -Wextra -pedantic -O2 -g

all: read
	./read

read: Makefile read.o catch_amalgamated.o
	${CXX} ${CFLAGS} ${CXXFLAGS} -o read read.o catch_amalgamated.o
