CC=g++
AR=ar

CFLAGS=-c -Wall -std=c++98
LIBFLAGS= -L. -lthreadpool
ARFLAGS=crf

all: libthreadpool.a

libthreadpool.a: threadpool.o
	$(AR) $(ARFLAGS) libthreadpool.a threadpool.o

threadpool.o: threadpool.cpp threadpool.h
	$(CC) $(CFLAGS) threadpool.cpp

test: libthreadpool.a test.cpp
	$(CC) -c -Wall -std=c++98 test.cpp -pthread
	$(CC) test.o $(LIBFLAGS) -pthread -o test

.PHONY: clean
clean:
	rm -rf *.[oa]
	rm -f test
