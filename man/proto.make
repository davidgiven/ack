# $Header$

#PARAMS		do not remove this line!

MANDIR = $(TARGET_HOME)/man

all:

install:
	if [ $(DO_MACHINE_INDEP) = y ] ; \
	then	if [ ! -d $(MANDIR) ] ; then mkdir $(MANDIR) ; fi ; \
		cd $(SRC_HOME)/man ; \
		for i in *.[1-8] ; do \
			num=`expr $$i : '.*\.\([1-8]\)'` ; \
			if [ ! -d $(MANDIR)/man$$num ] ; \
			then	mkdir $(MANDIR)/man$$num ; \
			fi ; \
			cat head $$i > $(MANDIR)/man$$num/$$i ; \
		done ; \
	fi

cmp:
opr:
pr:
clean:
