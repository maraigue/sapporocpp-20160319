#CCFLAGS=-std=c++11 -g -Wall -I$(HOME)/local/eigen-eigen-b30b87236a1b/Eigen
CCFLAGS=-std=c++11 -O3 -march=native -Wall -I$(HOME)/local/eigen-eigen-b30b87236a1b/Eigen
CC=g++

default:
	# Try "cat Makefile".

.cpp.o:
	$(CC) $(CCFLAGS) -c $<

1variable-minimization: 1variable-minimization.o
	$(CC) $(CCFLAGS) -o $@ $^

multi-variable-minimization: multi-variable-minimization.o
	$(CC) $(CCFLAGS) -o $@ $^

compute-border: compute-border.o numeric-csv-reader.o
	$(CC) $(CCFLAGS) -o $@ $^

clean:
	rm -vf *.exe
	rm -vf *.o
