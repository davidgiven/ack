# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/ego/ud
EMH=$(TARGET_HOME)/h
EMLIB=$(TARGET_HOME)/lib.bin

LDFLAGS=$(LDOPTIONS)
CPPFLAGS=-DVERBOSE -DNOTCOMPACT -I$(EMH) -I$(SRC_DIR) -I$(EMLIB)/ego
CFLAGS=$(CPPFLAGS) $(COPTIONS)
LINTFLAGS=$(CPPFLAGS) $(LINTOPTIONS)

CFILES=\
	$(SRC_DIR)/ud.c \
	$(SRC_DIR)/ud_defs.c \
	$(SRC_DIR)/ud_const.c \
	$(SRC_DIR)/ud_copy.c \
	$(SRC_DIR)/ud_aux.c

OFILES=\
	ud.$(SUF) ud_defs.$(SUF) ud_const.$(SUF) ud_copy.$(SUF) \
	ud_aux.$(SUF)

HFILES=\
	$(SRC_DIR)/ud.h \
	$(SRC_DIR)/ud_defs.h \
	$(SRC_DIR)/ud_const.h \
	$(SRC_DIR)/ud_copy.h \
	$(SRC_DIR)/ud_aux.h

PRFILES=\
	$(CFILES) $(HFILES) $(SRC_DIR)/proto.make

all:	ud

ud:	$(OFILES)
	$(CC) -o ud $(LDFLAGS) $(OFILES) $(EMLIB)/ego/share.$(LIBSUF) $(EMLIB)/em_data.$(LIBSUF)

install:	all
	cp ud $(EMLIB)/ego/ud

cmp:	all
	-cmp ud $(EMLIB)/ego/ud

clean:
	rm -f *.$(SUF) ud Out out nohup.out

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
ud.$(SUF):	$(SRC_DIR)/ud.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ud.c
ud.$(SUF):	$(SRC_DIR)/ud_copy.h
ud.$(SUF):	$(SRC_DIR)/ud_const.h
ud.$(SUF):	$(SRC_DIR)/ud_defs.h
ud.$(SUF):	$(SRC_DIR)/../share/go.h
ud.$(SUF):	$(SRC_DIR)/../share/locals.h
ud.$(SUF):	$(SRC_DIR)/../share/init_glob.h
ud.$(SUF):	$(SRC_DIR)/../share/aux.h
ud.$(SUF):	$(SRC_DIR)/../share/alloc.h
ud.$(SUF):	$(SRC_DIR)/../share/put.h
ud.$(SUF):	$(SRC_DIR)/../share/get.h
ud.$(SUF):	$(SRC_DIR)/../share/map.h
ud.$(SUF):	$(SRC_DIR)/../share/files.h
ud.$(SUF):	$(SRC_DIR)/../share/def.h
ud.$(SUF):	$(SRC_DIR)/../share/cset.h
ud.$(SUF):	$(SRC_DIR)/../share/lset.h
ud.$(SUF):	$(SRC_DIR)/../share/global.h
ud.$(SUF):	$(SRC_DIR)/../share/debug.h
ud.$(SUF):	$(SRC_DIR)/ud.h
ud.$(SUF):	$(SRC_DIR)/../share/types.h
ud.$(SUF):	$(EMH)/em_spec.h
ud_defs.$(SUF):	$(SRC_DIR)/ud_defs.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ud_defs.c
ud_defs.$(SUF):	$(SRC_DIR)/../share/aux.h
ud_defs.$(SUF):	$(SRC_DIR)/../share/alloc.h
ud_defs.$(SUF):	$(SRC_DIR)/ud_defs.h
ud_defs.$(SUF):	$(SRC_DIR)/../share/locals.h
ud_defs.$(SUF):	$(SRC_DIR)/../share/map.h
ud_defs.$(SUF):	$(SRC_DIR)/../share/cset.h
ud_defs.$(SUF):	$(SRC_DIR)/../share/lset.h
ud_defs.$(SUF):	$(SRC_DIR)/../share/global.h
ud_defs.$(SUF):	$(SRC_DIR)/../share/debug.h
ud_defs.$(SUF):	$(SRC_DIR)/ud.h
ud_defs.$(SUF):	$(SRC_DIR)/../share/types.h
ud_defs.$(SUF):	$(EMH)/em_mnem.h
ud_const.$(SUF):	$(SRC_DIR)/ud_const.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ud_const.c
ud_const.$(SUF):	$(SRC_DIR)/ud_aux.h
ud_const.$(SUF):	$(SRC_DIR)/ud_const.h
ud_const.$(SUF):	$(SRC_DIR)/ud_defs.h
ud_const.$(SUF):	$(SRC_DIR)/../share/locals.h
ud_const.$(SUF):	$(SRC_DIR)/../share/aux.h
ud_const.$(SUF):	$(SRC_DIR)/../share/def.h
ud_const.$(SUF):	$(SRC_DIR)/../share/cset.h
ud_const.$(SUF):	$(SRC_DIR)/../share/lset.h
ud_const.$(SUF):	$(SRC_DIR)/../share/alloc.h
ud_const.$(SUF):	$(SRC_DIR)/../share/global.h
ud_const.$(SUF):	$(SRC_DIR)/../share/debug.h
ud_const.$(SUF):	$(SRC_DIR)/ud.h
ud_const.$(SUF):	$(SRC_DIR)/../share/types.h
ud_const.$(SUF):	$(EMH)/em_spec.h
ud_const.$(SUF):	$(EMH)/em_pseu.h
ud_const.$(SUF):	$(EMH)/em_mnem.h
ud_copy.$(SUF):	$(SRC_DIR)/ud_copy.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ud_copy.c
ud_copy.$(SUF):	$(SRC_DIR)/ud_aux.h
ud_copy.$(SUF):	$(SRC_DIR)/ud_const.h
ud_copy.$(SUF):	$(SRC_DIR)/ud_copy.h
ud_copy.$(SUF):	$(SRC_DIR)/../ud/ud_defs.h
ud_copy.$(SUF):	$(SRC_DIR)/../share/locals.h
ud_copy.$(SUF):	$(SRC_DIR)/../share/aux.h
ud_copy.$(SUF):	$(SRC_DIR)/../share/def.h
ud_copy.$(SUF):	$(SRC_DIR)/../share/cset.h
ud_copy.$(SUF):	$(SRC_DIR)/../share/lset.h
ud_copy.$(SUF):	$(SRC_DIR)/../share/alloc.h
ud_copy.$(SUF):	$(SRC_DIR)/../share/global.h
ud_copy.$(SUF):	$(SRC_DIR)/../share/debug.h
ud_copy.$(SUF):	$(SRC_DIR)/ud.h
ud_copy.$(SUF):	$(SRC_DIR)/../share/types.h
ud_copy.$(SUF):	$(EMH)/em_spec.h
ud_copy.$(SUF):	$(EMH)/em_pseu.h
ud_copy.$(SUF):	$(EMH)/em_mnem.h
ud_aux.$(SUF):	$(SRC_DIR)/ud_aux.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ud_aux.c
ud_aux.$(SUF):	$(SRC_DIR)/ud_defs.h
ud_aux.$(SUF):	$(SRC_DIR)/../share/aux.h
ud_aux.$(SUF):	$(SRC_DIR)/../share/locals.h
ud_aux.$(SUF):	$(SRC_DIR)/../share/def.h
ud_aux.$(SUF):	$(SRC_DIR)/../share/cset.h
ud_aux.$(SUF):	$(SRC_DIR)/../share/lset.h
ud_aux.$(SUF):	$(SRC_DIR)/../share/alloc.h
ud_aux.$(SUF):	$(SRC_DIR)/../share/global.h
ud_aux.$(SUF):	$(SRC_DIR)/../share/debug.h
ud_aux.$(SUF):	$(SRC_DIR)/ud.h
ud_aux.$(SUF):	$(SRC_DIR)/../share/types.h
ud_aux.$(SUF):	$(EMH)/em_spec.h
ud_aux.$(SUF):	$(EMH)/em_pseu.h
ud_aux.$(SUF):	$(EMH)/em_mnem.h
