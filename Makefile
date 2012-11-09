# makefile for lua helper
# build on mac os, gcc 4.2.1

CC=g++
RM=rm

CFLAGS= -O2
LUA=-llua -L.

all:
	$(CC) -o debug_test.out debug_test.cpp debughelper.cpp $(CFLAGS) $(LUA)
	$(CC) -o reg_test.out reg_test.cpp $(CFLAGS) $(LUA)

clean:
	$(RM) -rf *.o *.out