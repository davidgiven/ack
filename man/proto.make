# $Header$

#PARAMS		do not remove this line!

MANDIR = $(TARGET_HOME)/man

all:

install:
	if [ $(DO_MACHINE_INDEP) = y ] ; \
	then	if [ ! -d $(MANDIR) ] ; then mkdir $(MANDIR) ; fi ; \
		cd $(SRC_HOME)/man ; \
		cp head $(MANDIR)/head ; \
		for i in *.[1-8] ; do \
			mk_manpage $$i $(TARGET_HOME) ; \
		done ; \
	fi

cmp:
opr:
pr:
clean:
