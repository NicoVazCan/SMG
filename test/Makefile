CC = gcc

CFLAGS  = -I ../include -g

SRC_SM=$(wildcard *.sm)

OBJ_SM=$(SRC_SM:.sm=.o)
HEAD_SM=$(SRC_SM:.sm=.h)

all: test

test: sm_loop.o sm_lib.o $(OBJ_SM)

sm_loop.c:
	./smg $(SRC_SM)

%.c: %.sm
	./smg $(SRC_SM)

clean:
	rm smg test *.o sm_loop.c $(HEAD_SM)