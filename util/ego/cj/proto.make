# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/ego/cj
EMH=$(TARGET_HOME)/h
EMLIB=$(TARGET_HOME)/lib.bin

LDFLAGS=$(LDOPTIONS)
CPPFLAGS=-DVERBOSE -DNOTCOMPACT -I$(EMH) -I$(SRC_DIR) -I$(EMLIB)/ego
CFLAGS=$(CPPFLAGS) $(COPTIONS)
LINTFLAGS=$(CPPFLAGS) $(LINTOPTIONS)

CFILES=\
	$(SRC_DIR)/cj.c

OFILES=\
cj.$(SUF)

HFILES=

PRFILES=\
	$(CFILES) $(HFILES) $(SRC_DIR)/proto.make

all:	cj

cj:	$(OFILES)
	$(CC) -o cj $(LDFLAGS) $(OFILES) $(EMLIB)/ego/share.$(LIBSUF) $(EMLIB)/em_data.$(LIBSUF)

install:	all
	cp cj $(EMLIB)/ego/cj

cmp:	all
	-cmp cj $(EMLIB)/ego/cj

clean:
	rm -f *.$(SUF) cj Out out nohup.out

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
cj.$(SUF):	$(SRC_DIR)/cj.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/cj.c
cj.$(SUF):	$(SRC_DIR)/../share/go.h
cj.$(SUF):	$(SRC_DIR)/../share/stack_chg.h
cj.$(SUF):	$(SRC_DIR)/../share/def.h
cj.$(SUF):	$(SRC_DIR)/../share/aux.h
cj.$(SUF):	$(SRC_DIR)/../share/alloc.h
cj.$(SUF):	$(SRC_DIR)/../share/map.h
cj.$(SUF):	$(SRC_DIR)/../share/lset.h
cj.$(SUF):	$(SRC_DIR)/../share/put.h
cj.$(SUF):	$(SRC_DIR)/../share/get.h
cj.$(SUF):	$(SRC_DIR)/../share/files.h
cj.$(SUF):	$(SRC_DIR)/../share/global.h
cj.$(SUF):	$(SRC_DIR)/../share/debug.h
cj.$(SUF):	$(SRC_DIR)/../share/types.h
cj.$(SUF):	$(EMH)/em_spec.h
cj.$(SUF):	$(EMH)/em_mnem.h
