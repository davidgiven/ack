# $Id$
O=.o
E=
CFLAGS=
LDFLAGS=
YFLAGS=
OBJS=iburg$O gram$O
CUSTOM=custom.mk
include $(CUSTOM)

iburg$E:	$(OBJS);	$(CC) -o $@ $(LDFLAGS) $(OBJS)

$(OBJS):	iburg.h

test:		iburg$E sample4.brg sample5.brg
		./iburg$E -I sample4.brg sample4.c; $(CC) -o test4$E sample4.c; ./test4$E
		./iburg$E -I sample5.brg sample5.c; $(CC) -o test5$E sample5.c; ./test5$E

clean::
		rm -f *$O core sample*.c a.out test4$E test5$E

clobber::	clean
		rm -f y.tab.c gram.tab.c iburg$E
