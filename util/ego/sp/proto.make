# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/ego/sp
EMH=$(TARGET_HOME)/h
EMLIB=$(TARGET_HOME)/lib.bin

LDFLAGS=$(LDOPTIONS)
CPPFLAGS=-DVERBOSE -DNOTCOMPACT -I$(EMH) -I$(SRC_DIR) -I$(EMLIB)/ego
CFLAGS=$(CPPFLAGS) $(COPTIONS)
LINTFLAGS=$(CPPFLAGS) $(LINTOPTIONS)

CFILES=\
	$(SRC_DIR)/sp.c

OFILES=\
sp.$(SUF)

HFILES=

PRFILES=\
	$(CFILES) $(HFILES) $(SRC_DIR)/proto.make

all:	sp

sp:	$(OFILES)
	$(CC) -o sp $(LDFLAGS) $(OFILES) $(EMLIB)/ego/share.$(LIBSUF) $(EMLIB)/em_data.$(LIBSUF)

install:	all
	cp sp $(EMLIB)/ego/sp

cmp:	all
	-cmp sp $(EMLIB)/ego/sp

clean:
	rm -f *.$(SUF) sp Out out nohup.out

lint:
	$(LINT) $(LINTFLAGS) $(CFILES) $(EMLIB)/ego/$(LINTPREF)share.$(LINTSUF) $(EMLIB)/$(LINTPREF)em_data.$(LINTSUF)

pr:
	@pr $(PRFILES)

opr:
	make pr | opr

depend:
	sed '/^#DEPENDENCIES/,$$d' Makefile >Makefile.new
	echo '#DEPENDENCIES' >>Makefile.new
	for i in $(CFILES) ; do \
		echo "`basename $$i .c`.$$(SUF):	$$i" >> Makefile.new ; \
		echo '	$$(CC) -c $$(CFLAGS)' $$i >> Makefile.new ; \
		$(UTIL_HOME)/lib.bin/cpp -d $(CPPFLAGS) $$i | sed "s/^/`basename $$i .c`.$$(SUF):	/" >> Makefile.new ; \
	done
	mv Makefile Makefile.old
	mv Makefile.new Makefile

# do not remove the next line
#DEPENDENCIES
sp.$(SUF):	$(SRC_DIR)/sp.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/sp.c
sp.$(SUF):	$(SRC_DIR)/../share/stack_chg.h
sp.$(SUF):	$(SRC_DIR)/../share/go.h
sp.$(SUF):	$(SRC_DIR)/../share/aux.h
sp.$(SUF):	$(SRC_DIR)/../share/alloc.h
sp.$(SUF):	$(SRC_DIR)/../share/map.h
sp.$(SUF):	$(SRC_DIR)/../share/lset.h
sp.$(SUF):	$(SRC_DIR)/../share/put.h
sp.$(SUF):	$(SRC_DIR)/../share/get.h
sp.$(SUF):	$(SRC_DIR)/../share/files.h
sp.$(SUF):	$(SRC_DIR)/../share/global.h
sp.$(SUF):	$(SRC_DIR)/../share/debug.h
sp.$(SUF):	$(SRC_DIR)/../share/types.h
sp.$(SUF):	$(EMH)/em_spec.h
sp.$(SUF):	$(EMH)/em_mnem.h
