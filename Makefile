CXXFLAGS+=-std=c++23 -Wall -Wextra -pedantic -O2 -g -I lib/ranges-v3/include

all: read plot
	./read --reporter xml > results.xml

read.o: lib/flux.hpp rangesv3

read: Makefile read.o catch_amalgamated.o
	${CXX} ${CFLAGS} ${CXXFLAGS} -o read read.o catch_amalgamated.o

lib/ranges-v3/build/:
	cd lib/ranges-v3/ && mkdir -p build && cd build && cmake ../

rangesv3: lib/ranges-v3/build/
	${MAKE} -C lib/ranges-v3/build

stats: read
	: > stats
	for t in clike algorithms boost_adaptors rangesv3 stdranges fluxranges; do \
		perf stat -o stats --append ./read "early single item" -c $$t --skip-benchmarks ; \
	done

stats.sc: stats
	stats.sc.pl < stats > stats.sc

.PHONY: plot
plot: Rplots.pdf

Rplots.pdf: results.xml
	Rscript plot.r results.xml

