CFLAGS=-Wall -Wextra
b: b0.o b1.o
	cc b0.o b1.o -o b
b0.o: b0.c b.h
b1.o: b1.c b.h

libs:
	./abc -c brt.s lib.b

install: b abc
	cp abc $(HOME)/bin

%.o: %.s
	as --32 $^ -o $@

