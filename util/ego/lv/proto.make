# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/ego/lv
EMH=$(TARGET_HOME)/h
EMLIB=$(TARGET_HOME)/lib.bin

LDFLAGS=$(LDOPTIONS)
CPPFLAGS=-DVERBOSE -DNOTCOMPACT -I$(EMH) -I$(SRC_DIR) -I$(EMLIB)/ego
CFLAGS=$(CPPFLAGS) $(COPTIONS)
LINTFLAGS=$(CPPFLAGS) $(LINTOPTIONS)

CFILES=\
	$(SRC_DIR)/lv.c

OFILES=\
lv.$(SUF)

HFILES=\
	$(SRC_DIR)/lv.h

PRFILES=\
	$(CFILES) $(HFILES) $(SRC_DIR)/proto.make

all:	lv

lv:	$(OFILES)
	$(CC) -o lv $(LDFLAGS) $(OFILES) $(EMLIB)/ego/share.$(LIBSUF) $(EMLIB)/em_data.$(LIBSUF)

install:	all
	cp lv $(EMLIB)/ego/lv

cmp:	all
	-cmp lv $(EMLIB)/ego/lv

clean:
	rm -f *.$(SUF) lv Out out nohup.out

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
lv.$(SUF):	$(SRC_DIR)/lv.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/lv.c
lv.$(SUF):	$(SRC_DIR)/../share/parser.h
lv.$(SUF):	$(SRC_DIR)/../share/go.h
lv.$(SUF):	$(SRC_DIR)/../share/locals.h
lv.$(SUF):	$(SRC_DIR)/../share/init_glob.h
lv.$(SUF):	$(SRC_DIR)/../share/aux.h
lv.$(SUF):	$(SRC_DIR)/../share/put.h
lv.$(SUF):	$(SRC_DIR)/../share/get.h
lv.$(SUF):	$(SRC_DIR)/../share/map.h
lv.$(SUF):	$(SRC_DIR)/../share/alloc.h
lv.$(SUF):	$(SRC_DIR)/../share/files.h
lv.$(SUF):	$(SRC_DIR)/../share/def.h
lv.$(SUF):	$(SRC_DIR)/../share/cset.h
lv.$(SUF):	$(SRC_DIR)/../share/lset.h
lv.$(SUF):	$(SRC_DIR)/../share/global.h
lv.$(SUF):	$(SRC_DIR)/../share/debug.h
lv.$(SUF):	$(SRC_DIR)/lv.h
lv.$(SUF):	$(SRC_DIR)/../share/types.h
lv.$(SUF):	$(EMH)/em_ego.h
lv.$(SUF):	$(EMH)/em_mes.h
lv.$(SUF):	$(EMH)/em_spec.h
lv.$(SUF):	$(EMH)/em_pseu.h
lv.$(SUF):	$(EMH)/em_mnem.h
