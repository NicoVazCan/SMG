
all:
	make -C src all
	cp src/smg  .

test: all
	cp smg test/
	make -C test all

clean:
	make -C src clean
	make -C test clean