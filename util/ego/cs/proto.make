# $Id$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/ego/cs
EMH=$(TARGET_HOME)/h
EMLIB=$(TARGET_HOME)/lib.bin

LDFLAGS=$(LDOPTIONS)
CPPFLAGS=-DVERBOSE -DNOTCOMPACT -I$(EMH) -I$(SRC_DIR) -I$(EMLIB)/ego
CFLAGS=$(CPPFLAGS) $(COPTIONS)
LINTFLAGS=$(CPPFLAGS) $(LINTOPTIONS)

CFILES=\
	$(SRC_DIR)/cs.c \
	$(SRC_DIR)/cs_alloc.c \
	$(SRC_DIR)/cs_aux.c \
	$(SRC_DIR)/cs_avail.c \
	$(SRC_DIR)/cs_debug.c \
	$(SRC_DIR)/cs_elim.c \
	$(SRC_DIR)/cs_entity.c \
	$(SRC_DIR)/cs_kill.c \
	$(SRC_DIR)/cs_partit.c \
	$(SRC_DIR)/cs_profit.c \
	$(SRC_DIR)/cs_getent.c \
	$(SRC_DIR)/cs_stack.c \
	$(SRC_DIR)/cs_vnm.c

OFILES=\
	cs.$(SUF) \
	cs_alloc.$(SUF) \
	cs_aux.$(SUF) \
	cs_avail.$(SUF) \
	cs_debug.$(SUF) \
	cs_elim.$(SUF) \
	cs_entity.$(SUF) \
	cs_kill.$(SUF) \
	cs_partit.$(SUF) \
	cs_profit.$(SUF) \
	cs_getent.$(SUF) \
	cs_stack.$(SUF) \
	cs_vnm.$(SUF)

HFILES=\
	$(SRC_DIR)/cs.h \
	$(SRC_DIR)/cs_alloc.h \
	$(SRC_DIR)/cs_aux.h \
	$(SRC_DIR)/cs_avail.h \
	$(SRC_DIR)/cs_debug.h \
	$(SRC_DIR)/cs_elim.h \
	$(SRC_DIR)/cs_entity.h \
	$(SRC_DIR)/cs_kill.h \
	$(SRC_DIR)/cs_partit.h \
	$(SRC_DIR)/cs_profit.h \
	$(SRC_DIR)/cs_getent.h \
	$(SRC_DIR)/cs_stack.h \
	$(SRC_DIR)/cs_vnm.h

PRFILES=\
	$(CFILES) $(HFILES) $(SRC_DIR)/proto.make

all:	cs

cs:	$(OFILES)
	$(CC) -o cs $(LDFLAGS) $(OFILES) $(EMLIB)/ego/share.$(LIBSUF) $(EMLIB)/em_data.$(LIBSUF)

install:	all
	cp cs $(EMLIB)/ego/cs

cmp:	all
	-cmp cs $(EMLIB)/ego/cs

clean:
	rm -f *.$(SUF) cs Out out nohup.out

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
cs.$(SUF):	$(SRC_DIR)/cs.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/cs.c
cs.$(SUF):	$(SRC_DIR)/cs_vnm.h
cs.$(SUF):	$(SRC_DIR)/cs_stack.h
cs.$(SUF):	$(SRC_DIR)/cs_profit.h
cs.$(SUF):	$(SRC_DIR)/cs_entity.h
cs.$(SUF):	$(SRC_DIR)/cs_elim.h
cs.$(SUF):	$(SRC_DIR)/cs_debug.h
cs.$(SUF):	$(SRC_DIR)/cs_avail.h
cs.$(SUF):	$(SRC_DIR)/cs_aux.h
cs.$(SUF):	$(SRC_DIR)/cs.h
cs.$(SUF):	$(SRC_DIR)/../share/go.h
cs.$(SUF):	$(SRC_DIR)/../share/debug.h
cs.$(SUF):	$(SRC_DIR)/../share/lset.h
cs.$(SUF):	$(SRC_DIR)/../share/types.h
cs_alloc.$(SUF):	$(SRC_DIR)/cs_alloc.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/cs_alloc.c
cs_alloc.$(SUF):	$(SRC_DIR)/cs.h
cs_alloc.$(SUF):	$(SRC_DIR)/../share/alloc.h
cs_alloc.$(SUF):	$(SRC_DIR)/../share/types.h
cs_aux.$(SUF):	$(SRC_DIR)/cs_aux.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/cs_aux.c
cs_aux.$(SUF):	$(SRC_DIR)/cs_entity.h
cs_aux.$(SUF):	$(SRC_DIR)/cs.h
cs_aux.$(SUF):	$(SRC_DIR)/../share/lset.h
cs_aux.$(SUF):	$(SRC_DIR)/../share/global.h
cs_aux.$(SUF):	$(SRC_DIR)/../share/aux.h
cs_aux.$(SUF):	$(SRC_DIR)/../share/debug.h
cs_aux.$(SUF):	$(SRC_DIR)/../share/types.h
cs_avail.$(SUF):	$(SRC_DIR)/cs_avail.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/cs_avail.c
cs_avail.$(SUF):	$(SRC_DIR)/cs_getent.h
cs_avail.$(SUF):	$(SRC_DIR)/cs_alloc.h
cs_avail.$(SUF):	$(SRC_DIR)/cs_debug.h
cs_avail.$(SUF):	$(SRC_DIR)/cs_aux.h
cs_avail.$(SUF):	$(SRC_DIR)/cs.h
cs_avail.$(SUF):	$(SRC_DIR)/../share/global.h
cs_avail.$(SUF):	$(SRC_DIR)/../share/lset.h
cs_avail.$(SUF):	$(SRC_DIR)/../share/aux.h
cs_avail.$(SUF):	$(SRC_DIR)/../share/debug.h
cs_avail.$(SUF):	$(SRC_DIR)/../share/types.h
cs_avail.$(SUF):	$(EMH)/em_mnem.h
cs_debug.$(SUF):	$(SRC_DIR)/cs_debug.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/cs_debug.c
cs_debug.$(SUF):	$(SRC_DIR)/cs_entity.h
cs_debug.$(SUF):	$(SRC_DIR)/cs_avail.h
cs_debug.$(SUF):	$(SRC_DIR)/cs_aux.h
cs_debug.$(SUF):	$(SRC_DIR)/cs.h
cs_debug.$(SUF):	$(SRC_DIR)/../share/lset.h
cs_debug.$(SUF):	$(SRC_DIR)/../share/debug.h
cs_debug.$(SUF):	$(SRC_DIR)/../share/types.h
cs_debug.$(SUF):	$(EMH)/em_spec.h
cs_elim.$(SUF):	$(SRC_DIR)/cs_elim.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/cs_elim.c
cs_elim.$(SUF):	$(SRC_DIR)/cs_partit.h
cs_elim.$(SUF):	$(SRC_DIR)/cs_profit.h
cs_elim.$(SUF):	$(SRC_DIR)/cs_debug.h
cs_elim.$(SUF):	$(SRC_DIR)/cs_aux.h
cs_elim.$(SUF):	$(SRC_DIR)/cs_alloc.h
cs_elim.$(SUF):	$(SRC_DIR)/cs_avail.h
cs_elim.$(SUF):	$(SRC_DIR)/cs.h
cs_elim.$(SUF):	$(SRC_DIR)/../share/debug.h
cs_elim.$(SUF):	$(SRC_DIR)/../share/global.h
cs_elim.$(SUF):	$(SRC_DIR)/../share/aux.h
cs_elim.$(SUF):	$(SRC_DIR)/../share/lset.h
cs_elim.$(SUF):	$(SRC_DIR)/../share/alloc.h
cs_elim.$(SUF):	$(SRC_DIR)/../share/types.h
cs_elim.$(SUF):	$(EMH)/em_mnem.h
cs_elim.$(SUF):	$(EMH)/em_reg.h
cs_entity.$(SUF):	$(SRC_DIR)/cs_entity.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/cs_entity.c
cs_entity.$(SUF):	$(SRC_DIR)/cs_aux.h
cs_entity.$(SUF):	$(SRC_DIR)/cs_alloc.h
cs_entity.$(SUF):	$(SRC_DIR)/cs.h
cs_entity.$(SUF):	$(SRC_DIR)/../share/debug.h
cs_entity.$(SUF):	$(SRC_DIR)/../share/lset.h
cs_entity.$(SUF):	$(SRC_DIR)/../share/global.h
cs_entity.$(SUF):	$(SRC_DIR)/../share/types.h
cs_kill.$(SUF):	$(SRC_DIR)/cs_kill.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/cs_kill.c
cs_kill.$(SUF):	$(SRC_DIR)/cs_entity.h
cs_kill.$(SUF):	$(SRC_DIR)/cs_avail.h
cs_kill.$(SUF):	$(SRC_DIR)/cs_debug.h
cs_kill.$(SUF):	$(SRC_DIR)/cs_aux.h
cs_kill.$(SUF):	$(SRC_DIR)/cs.h
cs_kill.$(SUF):	$(SRC_DIR)/../share/map.h
cs_kill.$(SUF):	$(SRC_DIR)/../share/aux.h
cs_kill.$(SUF):	$(SRC_DIR)/../share/cset.h
cs_kill.$(SUF):	$(SRC_DIR)/../share/lset.h
cs_kill.$(SUF):	$(SRC_DIR)/../share/global.h
cs_kill.$(SUF):	$(SRC_DIR)/../share/debug.h
cs_kill.$(SUF):	$(SRC_DIR)/../share/types.h
cs_kill.$(SUF):	$(EMH)/em_mnem.h
cs_partit.$(SUF):	$(SRC_DIR)/cs_partit.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/cs_partit.c
cs_partit.$(SUF):	$(SRC_DIR)/cs_stack.h
cs_partit.$(SUF):	$(SRC_DIR)/cs.h
cs_partit.$(SUF):	$(SRC_DIR)/../share/global.h
cs_partit.$(SUF):	$(SRC_DIR)/../share/debug.h
cs_partit.$(SUF):	$(SRC_DIR)/../share/aux.h
cs_partit.$(SUF):	$(SRC_DIR)/../share/types.h
cs_partit.$(SUF):	$(EMH)/em_spec.h
cs_partit.$(SUF):	$(EMH)/em_reg.h
cs_partit.$(SUF):	$(EMH)/em_pseu.h
cs_partit.$(SUF):	$(EMH)/em_mnem.h
cs_profit.$(SUF):	$(SRC_DIR)/cs_profit.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/cs_profit.c
cs_profit.$(SUF):	$(SRC_DIR)/cs_partit.h
cs_profit.$(SUF):	$(SRC_DIR)/cs_avail.h
cs_profit.$(SUF):	$(SRC_DIR)/cs_debug.h
cs_profit.$(SUF):	$(SRC_DIR)/cs_aux.h
cs_profit.$(SUF):	$(SRC_DIR)/cs.h
cs_profit.$(SUF):	$(SRC_DIR)/../share/lset.h
cs_profit.$(SUF):	$(SRC_DIR)/../share/cset.h
cs_profit.$(SUF):	$(SRC_DIR)/../share/aux.h
cs_profit.$(SUF):	$(SRC_DIR)/../share/global.h
cs_profit.$(SUF):	$(SRC_DIR)/../share/debug.h
cs_profit.$(SUF):	$(SRC_DIR)/../share/types.h
cs_profit.$(SUF):	$(EMH)/em_spec.h
cs_profit.$(SUF):	$(EMH)/em_mnem.h
cs_getent.$(SUF):	$(SRC_DIR)/cs_getent.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/cs_getent.c
cs_getent.$(SUF):	$(SRC_DIR)/cs_stack.h
cs_getent.$(SUF):	$(SRC_DIR)/cs_entity.h
cs_getent.$(SUF):	$(SRC_DIR)/cs_aux.h
cs_getent.$(SUF):	$(SRC_DIR)/cs.h
cs_getent.$(SUF):	$(SRC_DIR)/../share/global.h
cs_getent.$(SUF):	$(SRC_DIR)/../share/debug.h
cs_getent.$(SUF):	$(SRC_DIR)/../share/aux.h
cs_getent.$(SUF):	$(SRC_DIR)/../share/types.h
cs_getent.$(SUF):	$(EMH)/em_mnem.h
cs_stack.$(SUF):	$(SRC_DIR)/cs_stack.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/cs_stack.c
cs_stack.$(SUF):	$(SRC_DIR)/cs_aux.h
cs_stack.$(SUF):	$(SRC_DIR)/cs.h
cs_stack.$(SUF):	$(SRC_DIR)/../share/aux.h
cs_stack.$(SUF):	$(SRC_DIR)/../share/debug.h
cs_stack.$(SUF):	$(SRC_DIR)/../share/global.h
cs_stack.$(SUF):	$(SRC_DIR)/../share/types.h
cs_vnm.$(SUF):	$(SRC_DIR)/cs_vnm.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/cs_vnm.c
cs_vnm.$(SUF):	$(SRC_DIR)/cs_getent.h
cs_vnm.$(SUF):	$(SRC_DIR)/cs_partit.h
cs_vnm.$(SUF):	$(SRC_DIR)/cs_kill.h
cs_vnm.$(SUF):	$(SRC_DIR)/cs_stack.h
cs_vnm.$(SUF):	$(SRC_DIR)/cs_avail.h
cs_vnm.$(SUF):	$(SRC_DIR)/cs_entity.h
cs_vnm.$(SUF):	$(SRC_DIR)/cs_aux.h
cs_vnm.$(SUF):	$(SRC_DIR)/cs_alloc.h
cs_vnm.$(SUF):	$(SRC_DIR)/cs.h
cs_vnm.$(SUF):	$(SRC_DIR)/../share/aux.h
cs_vnm.$(SUF):	$(SRC_DIR)/../share/debug.h
cs_vnm.$(SUF):	$(SRC_DIR)/../share/global.h
cs_vnm.$(SUF):	$(SRC_DIR)/../share/types.h
cs_vnm.$(SUF):	$(EMH)/em_mnem.h
