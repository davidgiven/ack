# $Id$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/ego/ic
EMH=$(TARGET_HOME)/h
EMLIB=$(TARGET_HOME)/lib.bin

LDFLAGS=$(LDOPTIONS)
CPPFLAGS=-DVERBOSE -DNOTCOMPACT -I$(EMH) -I$(SRC_DIR) -I$(EMLIB)/ego
CFLAGS=$(CPPFLAGS) $(COPTIONS)
LINTFLAGS=$(CPPFLAGS) $(LINTOPTIONS)

CFILES=\
	$(SRC_DIR)/ic.c \
	$(SRC_DIR)/ic_aux.c \
	$(SRC_DIR)/ic_lib.c \
	$(SRC_DIR)/ic_lookup.c \
	$(SRC_DIR)/ic_io.c

OFILES=\
ic.$(SUF) ic_aux.$(SUF) ic_lookup.$(SUF) ic_io.$(SUF) ic_lib.$(SUF)

HFILES=\
	$(SRC_DIR)/ic.h \
	$(SRC_DIR)/ic_aux.h \
	$(SRC_DIR)/ic_lib.h \
	$(SRC_DIR)/ic_lookup.h \
	$(SRC_DIR)/ic_io.h

PRFILES=\
	$(CFILES) $(HFILES) $(SRC_DIR)/proto.make

all:	ic

ic:	$(OFILES)
	$(CC) -o ic $(LDFLAGS) $(OFILES) $(EMLIB)/ego/share.$(LIBSUF) $(EMLIB)/em_data.$(LIBSUF)

install:	all
	cp ic $(EMLIB)/ego/ic

cmp:	all
	-cmp ic $(EMLIB)/ego/ic

clean:
	rm -f *.$(SUF) ic Out out nohup.out

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
ic.$(SUF):	$(SRC_DIR)/ic.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ic.c
ic.$(SUF):	$(SRC_DIR)/../share/aux.h
ic.$(SUF):	$(SRC_DIR)/../share/put.h
ic.$(SUF):	$(SRC_DIR)/../share/files.h
ic.$(SUF):	$(SRC_DIR)/../share/global.h
ic.$(SUF):	$(SRC_DIR)/../share/alloc.h
ic.$(SUF):	$(SRC_DIR)/ic_lib.h
ic.$(SUF):	$(SRC_DIR)/ic_io.h
ic.$(SUF):	$(SRC_DIR)/ic_aux.h
ic.$(SUF):	$(SRC_DIR)/ic_lookup.h
ic.$(SUF):	$(SRC_DIR)/ic.h
ic.$(SUF):	$(SRC_DIR)/../share/map.h
ic.$(SUF):	$(SRC_DIR)/../share/def.h
ic.$(SUF):	$(SRC_DIR)/../share/debug.h
ic.$(SUF):	$(SRC_DIR)/../share/types.h
ic.$(SUF):	$(EMH)/em_mes.h
ic.$(SUF):	$(EMH)/em_flag.h
ic.$(SUF):	$(EMH)/em_pseu.h
ic.$(SUF):	$(EMH)/em_spec.h
ic_aux.$(SUF):	$(SRC_DIR)/ic_aux.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ic_aux.c
ic_aux.$(SUF):	$(SRC_DIR)/ic_aux.h
ic_aux.$(SUF):	$(SRC_DIR)/../share/alloc.h
ic_aux.$(SUF):	$(SRC_DIR)/ic_lookup.h
ic_aux.$(SUF):	$(SRC_DIR)/ic_io.h
ic_aux.$(SUF):	$(SRC_DIR)/ic.h
ic_aux.$(SUF):	$(SRC_DIR)/../share/aux.h
ic_aux.$(SUF):	$(SRC_DIR)/../share/def.h
ic_aux.$(SUF):	$(SRC_DIR)/../share/debug.h
ic_aux.$(SUF):	$(SRC_DIR)/../share/global.h
ic_aux.$(SUF):	$(SRC_DIR)/../share/types.h
ic_aux.$(SUF):	$(EMH)/em_mnem.h
ic_aux.$(SUF):	$(EMH)/em_spec.h
ic_aux.$(SUF):	$(EMH)/em_pseu.h
ic_lib.$(SUF):	$(SRC_DIR)/ic_lib.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ic_lib.c
ic_lib.$(SUF):	$(SRC_DIR)/ic_lib.h
ic_lib.$(SUF):	$(SRC_DIR)/../share/files.h
ic_lib.$(SUF):	$(SRC_DIR)/../share/global.h
ic_lib.$(SUF):	$(SRC_DIR)/ic_io.h
ic_lib.$(SUF):	$(SRC_DIR)/ic_lookup.h
ic_lib.$(SUF):	$(SRC_DIR)/ic.h
ic_lib.$(SUF):	$(SRC_DIR)/../share/debug.h
ic_lib.$(SUF):	$(SRC_DIR)/../share/types.h
ic_lib.$(SUF):	$(EMH)/arch.h
ic_lib.$(SUF):	$(EMH)/em_mes.h
ic_lib.$(SUF):	$(EMH)/em_pseu.h
ic_lib.$(SUF):	$(EMH)/em_spec.h
ic_lookup.$(SUF):	$(SRC_DIR)/ic_lookup.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ic_lookup.c
ic_lookup.$(SUF):	$(SRC_DIR)/../share/alloc.h
ic_lookup.$(SUF):	$(SRC_DIR)/ic_lookup.h
ic_lookup.$(SUF):	$(SRC_DIR)/ic.h
ic_lookup.$(SUF):	$(SRC_DIR)/../share/map.h
ic_lookup.$(SUF):	$(SRC_DIR)/../share/debug.h
ic_lookup.$(SUF):	$(SRC_DIR)/../share/types.h
ic_lookup.$(SUF):	$(EMH)/em_spec.h
ic_io.$(SUF):	$(SRC_DIR)/ic_io.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ic_io.c
ic_io.$(SUF):	$(SRC_DIR)/ic_io.h
ic_io.$(SUF):	$(SRC_DIR)/../share/alloc.h
ic_io.$(SUF):	$(SRC_DIR)/ic_lookup.h
ic_io.$(SUF):	$(SRC_DIR)/ic.h
ic_io.$(SUF):	$(SRC_DIR)/../share/debug.h
ic_io.$(SUF):	$(SRC_DIR)/../share/types.h
ic_io.$(SUF):	$(EMH)/arch.h
ic_io.$(SUF):	$(EMH)/em_spec.h
ic_io.$(SUF):	$(EMH)/em_pseu.h
