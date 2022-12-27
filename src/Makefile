BUILD = release
cflags.release = -O3
cflags.debug = -O0 -g

CC = gcc
LEX = flex
YACC = bison

CFLAGS  = $(cflags.$(BUILD)) -I ../include
LDFLAGS = -Lfl -Ly
YFLAGS  = -yd

all: smg

smg: parser.o scanner.o terms.o

clean:
	rm *.o y.tab.h
