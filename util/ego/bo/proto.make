# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/ego/bo
EMH=$(TARGET_HOME)/h
EMLIB=$(TARGET_HOME)/lib.bin

LDFLAGS=$(LDOPTIONS)
CPPFLAGS=-DVERBOSE -DNOTCOMPACT -I$(EMH) -I$(SRC_DIR) -I$(EMLIB)/ego
CFLAGS=$(CPPFLAGS) $(COPTIONS)
LINTFLAGS=$(CPPFLAGS) $(LINTOPTIONS)

CFILES=\
	$(SRC_DIR)/bo.c

OFILES=\
bo.$(SUF)

HFILES=

PRFILES=\
	$(CFILES) $(HFILES) $(SRC_DIR)/proto.make

all:	bo

bo:	$(OFILES)
	$(CC) -o bo $(LDFLAGS) $(OFILES) $(EMLIB)/ego/share.$(LIBSUF) $(EMLIB)/em_data.$(LIBSUF)

install:	all
	cp bo $(EMLIB)/ego/bo

cmp:	all
	-cmp bo $(EMLIB)/ego/bo

clean:
	rm -f *.$(SUF) bo Out out nohup.out

lint:
	$(LINT) $(LINTFLAGS) $(CFILES) $(EMLIB)/ego/$(LINTPREF)share.$(LINTSUF) $(EMLIB)/$(LINTPREF)em_data.$(LINTSUF)

pr:
	@pr $(PRFILES)

opr:
	make pr | opr

depend:
	rm_deps Makefile >Makefile.new
	for i in $(CFILES) ; do \
		echo "`basename $$i .c`.$$(SUF):	$$i" >> Makefile.new ; \
		echo '	$$(CC) -c $$(CFLAGS)' $$i >> Makefile.new ; \
		$(UTIL_HOME)/lib.bin/cpp -d $(CPPFLAGS) $$i | sed "s/^/`basename $$i .c`.$$(SUF):	/" >> Makefile.new ; \
	done
	mv Makefile Makefile.old
	mv Makefile.new Makefile

# do not remove the next line
#DEPENDENCIES
bo.$(SUF):	$(SRC_DIR)/bo.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/bo.c
bo.$(SUF):	$(SRC_DIR)/../share/go.h
bo.$(SUF):	$(SRC_DIR)/../share/def.h
bo.$(SUF):	$(SRC_DIR)/../share/aux.h
bo.$(SUF):	$(SRC_DIR)/../share/alloc.h
bo.$(SUF):	$(SRC_DIR)/../share/map.h
bo.$(SUF):	$(SRC_DIR)/../share/lset.h
bo.$(SUF):	$(SRC_DIR)/../share/put.h
bo.$(SUF):	$(SRC_DIR)/../share/get.h
bo.$(SUF):	$(SRC_DIR)/../share/files.h
bo.$(SUF):	$(SRC_DIR)/../share/global.h
bo.$(SUF):	$(SRC_DIR)/../share/debug.h
bo.$(SUF):	$(SRC_DIR)/../share/types.h
bo.$(SUF):	$(EMH)/em_flag.h
bo.$(SUF):	$(EMH)/em_spec.h
bo.$(SUF):	$(EMH)/em_pseu.h
bo.$(SUF):	$(EMH)/em_mnem.h
