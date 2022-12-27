CC = gcc

CFLAGS  = -I ../include

all: test

test: sm1.o sm0.o sm_lib.o sm_loop.o

%.c: %.sm
	./smg *.sm

%.h: %.sm
	./smg *.sm

clean:
	rm *.o sm_loop.c sm*.c sm*.h