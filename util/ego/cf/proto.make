# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/ego/cf
EMH=$(TARGET_HOME)/h
EMLIB=$(TARGET_HOME)/lib.bin

LDFLAGS=$(LDOPTIONS)
CPPFLAGS=-DVERBOSE -DNOTCOMPACT -I$(EMH) -I$(SRC_DIR) -I$(EMLIB)/ego
CFLAGS=$(CPPFLAGS) $(COPTIONS)
LINTFLAGS=$(CPPFLAGS) $(LINTOPTIONS)

CFILES=\
	$(SRC_DIR)/cf.c \
	$(SRC_DIR)/cf_succ.c \
	$(SRC_DIR)/cf_idom.c \
	$(SRC_DIR)/cf_loop.c

OFILES=\
cf.$(SUF) cf_succ.$(SUF) cf_idom.$(SUF) cf_loop.$(SUF)

HFILES=\
	$(SRC_DIR)/cf.h \
	$(SRC_DIR)/cf_succ.h \
	$(SRC_DIR)/cf_idom.h \
	$(SRC_DIR)/cf_loop.h

PRFILES=\
	$(CFILES) $(HFILES) $(SRC_DIR)/proto.make

all:	cf

cf:	$(OFILES)
	$(CC) -o cf $(LDFLAGS) $(OFILES) $(EMLIB)/ego/share.$(LIBSUF) $(EMLIB)/em_data.$(LIBSUF)

install:	all
	cp cf $(EMLIB)/ego/cf

cmp:	all
	-cmp cf $(EMLIB)/ego/cf

clean:
	rm -f *.$(SUF) cf Out out nohup.out

lint:
	$(LINT) $(LINTFLAGS) $(CFILES)

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
cf.$(SUF):	$(SRC_DIR)/cf.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/cf.c
cf.$(SUF):	$(SRC_DIR)/cf_loop.h
cf.$(SUF):	$(SRC_DIR)/cf_idom.h
cf.$(SUF):	$(SRC_DIR)/cf_succ.h
cf.$(SUF):	$(SRC_DIR)/cf.h
cf.$(SUF):	$(SRC_DIR)/../share/def.h
cf.$(SUF):	$(SRC_DIR)/../share/put.h
cf.$(SUF):	$(SRC_DIR)/../share/get.h
cf.$(SUF):	$(SRC_DIR)/../share/cset.h
cf.$(SUF):	$(SRC_DIR)/../share/lset.h
cf.$(SUF):	$(SRC_DIR)/../share/alloc.h
cf.$(SUF):	$(SRC_DIR)/../share/global.h
cf.$(SUF):	$(SRC_DIR)/../share/files.h
cf.$(SUF):	$(SRC_DIR)/../share/map.h
cf.$(SUF):	$(SRC_DIR)/../share/debug.h
cf.$(SUF):	$(SRC_DIR)/../share/types.h
cf.$(SUF):	$(EMH)/em_mes.h
cf.$(SUF):	$(EMH)/em_flag.h
cf.$(SUF):	$(EMH)/em_spec.h
cf.$(SUF):	$(EMH)/em_pseu.h
cf.$(SUF):	$(EMH)/em_mnem.h
cf_succ.$(SUF):	$(SRC_DIR)/cf_succ.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/cf_succ.c
cf_succ.$(SUF):	$(SRC_DIR)/../share/map.h
cf_succ.$(SUF):	$(SRC_DIR)/cf.h
cf_succ.$(SUF):	$(SRC_DIR)/../share/cset.h
cf_succ.$(SUF):	$(SRC_DIR)/../share/lset.h
cf_succ.$(SUF):	$(SRC_DIR)/../share/global.h
cf_succ.$(SUF):	$(SRC_DIR)/../share/debug.h
cf_succ.$(SUF):	$(SRC_DIR)/../share/def.h
cf_succ.$(SUF):	$(SRC_DIR)/../share/types.h
cf_succ.$(SUF):	$(EMH)/em_mnem.h
cf_succ.$(SUF):	$(EMH)/em_flag.h
cf_succ.$(SUF):	$(EMH)/em_pseu.h
cf_succ.$(SUF):	$(EMH)/em_spec.h
cf_idom.$(SUF):	$(SRC_DIR)/cf_idom.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/cf_idom.c
cf_idom.$(SUF):	$(SRC_DIR)/cf.h
cf_idom.$(SUF):	$(SRC_DIR)/../share/alloc.h
cf_idom.$(SUF):	$(SRC_DIR)/../share/lset.h
cf_idom.$(SUF):	$(SRC_DIR)/../share/debug.h
cf_idom.$(SUF):	$(SRC_DIR)/../share/types.h
cf_loop.$(SUF):	$(SRC_DIR)/cf_loop.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/cf_loop.c
cf_loop.$(SUF):	$(SRC_DIR)/cf.h
cf_loop.$(SUF):	$(SRC_DIR)/../share/aux.h
cf_loop.$(SUF):	$(SRC_DIR)/../share/alloc.h
cf_loop.$(SUF):	$(SRC_DIR)/../share/lset.h
cf_loop.$(SUF):	$(SRC_DIR)/../share/debug.h
cf_loop.$(SUF):	$(SRC_DIR)/../share/types.h
