# $Header$

#PARAMS		do not remove this line!

DIRS = lpass1 lpass1.ansi lpass2 llib

all:
	for i in $(DIRS) ; do cd $$i ; make ; cd .. ; done

install:
	for i in $(DIRS) ; do cd $$i ; make install ; cd .. ; done

clean:
	for i in $(DIRS) ; do cd $$i ; make clean ; cd .. ; done

cmp:
	for i in $(DIRS) ; do cd $$i ; make cmp ; cd .. ; done
