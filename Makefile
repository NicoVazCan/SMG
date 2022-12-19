BUILD = debug
cflags.release = -O3
cflags.debug = -O0 -g

CC = gcc
LEX = flex
YACC = bison

CFLAGS  = $(cflags.$(BUILD)) -I include -I .
LDFLAGS = -Lfl -Ly
YFLAGS  = -yd

all: src/smg build

src/smg: src/parser.o src/scanner.o src/terms.o

build:
	mv src/smg .

clean:
	rm src/*.o y.tab.h
