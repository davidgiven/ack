head:	mccpm nascom

mccpm:	mccpm.c
	cc -o mccpm mccpm.c

nascom:	nascom.c
	cc -o nascom nascom.c

install:
	@echo Nothing is installed

clean:
	rm nascom mccpm

pr:
	@pr `pwd`/Makefile `pwd`/mccpm.c `pwd`/nascom.c

opr:
	make pr | opr
