# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/ego/ra
EMH=$(TARGET_HOME)/h
EMLIB=$(TARGET_HOME)/lib.bin

LDFLAGS=$(LDOPTIONS)
ULDFLAGS=$(ULDOPTIONS)
CPPFLAGS=-DVERBOSE -DNOTCOMPACT -I$(EMH) -I$(SRC_DIR) -I$(EMLIB)/ego -I.
CFLAGS=$(CPPFLAGS) $(COPTIONS)
UCFLAGS=$(CPPFLAGS) $(UCOPTIONS)
LINTFLAGS=$(CPPFLAGS) $(LINTOPTIONS)

CFILES=\
	$(SRC_DIR)/ra.c \
	$(SRC_DIR)/ra_items.c \
	$(SRC_DIR)/ra_lifet.c \
	$(SRC_DIR)/ra_allocl.c \
	$(SRC_DIR)/ra_profits.c \
	$(SRC_DIR)/ra_interv.c \
	$(SRC_DIR)/ra_pack.c \
	$(SRC_DIR)/ra_xform.c \
	$(SRC_DIR)/ra_aux.c

OFILES=\
	ra.$(SUF) \
	ra_items.$(SUF) \
	ra_lifet.$(SUF) \
	ra_allocl.$(SUF) \
	ra_profits.$(SUF) \
	ra_interv.$(SUF) \
	ra_pack.$(SUF) \
	ra_xform.$(SUF) \
	ra_aux.$(SUF)

HFILES=\
	$(SRC_DIR)/ra.h \
	$(SRC_DIR)/ra_items.h \
	$(SRC_DIR)/ra_lifet.h \
	$(SRC_DIR)/ra_allocl.h \
	$(SRC_DIR)/ra_profits.h \
	$(SRC_DIR)/ra_interv.h \
	$(SRC_DIR)/ra_pack.h \
	$(SRC_DIR)/ra_xform.h \
	$(SRC_DIR)/ra_aux.h

PRFILES=\
	$(CFILES) $(HFILES) $(SRC_DIR)/proto.make

all:	ra

ra:	$(OFILES)
	$(CC) -o ra $(LDFLAGS) $(OFILES) $(EMLIB)/ego/share.$(LIBSUF) $(EMLIB)/em_data.$(LIBSUF)

./itemtab.h:	$(SRC_DIR)/itemtab.src $(EMH)/em_mnem.h makeitems
		makeitems $(EMH)/em_mnem.h $(SRC_DIR)/itemtab.src > ./itemtab.h

makeitems:	$(SRC_DIR)/makeitems.c
		$(UCC) $(UCFLAGS) $(ULDFLAGS) -o makeitems $(SRC_DIR)/makeitems.c

install:	all
	cp ra $(EMLIB)/ego/ra

cmp:	all
	-cmp ra $(EMLIB)/ego/ra

clean:
	rm -f *.$(SUF) ra Out out nohup.out makeitems ./itemtab.h

lint:
	$(LINT) $(LINTFLAGS) $(CFILES) $(EMLIB)/ego/$(LINTPREF)share.$(LINTSUF) $(EMLIB)/$(LINTPREF)em_data.$(LINTSUF)

pr:
	@pr $(PRFILES)

opr:
	make pr | opr

depend:	./itemtab.h
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
ra.$(SUF):	$(SRC_DIR)/ra.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ra.c
ra.$(SUF):	$(SRC_DIR)/ra_xform.h
ra.$(SUF):	$(SRC_DIR)/ra_pack.h
ra.$(SUF):	$(SRC_DIR)/ra_profits.h
ra.$(SUF):	$(SRC_DIR)/ra_allocl.h
ra.$(SUF):	$(SRC_DIR)/ra_items.h
ra.$(SUF):	$(SRC_DIR)/ra.h
ra.$(SUF):	$(SRC_DIR)/../share/go.h
ra.$(SUF):	$(SRC_DIR)/../share/alloc.h
ra.$(SUF):	$(SRC_DIR)/../share/map.h
ra.$(SUF):	$(SRC_DIR)/../share/lset.h
ra.$(SUF):	$(SRC_DIR)/../share/put.h
ra.$(SUF):	$(SRC_DIR)/../share/get.h
ra.$(SUF):	$(SRC_DIR)/../share/files.h
ra.$(SUF):	$(SRC_DIR)/../share/global.h
ra.$(SUF):	$(SRC_DIR)/../share/debug.h
ra.$(SUF):	$(SRC_DIR)/../share/types.h
ra.$(SUF):	$(EMH)/em_reg.h
ra_items.$(SUF):	$(SRC_DIR)/ra_items.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ra_items.c
ra_items.$(SUF):	./itemtab.h
ra_items.$(SUF):	$(SRC_DIR)/ra_items.h
ra_items.$(SUF):	$(SRC_DIR)/ra_aux.h
ra_items.$(SUF):	$(SRC_DIR)/ra.h
ra_items.$(SUF):	$(SRC_DIR)/../share/alloc.h
ra_items.$(SUF):	$(SRC_DIR)/../share/aux.h
ra_items.$(SUF):	$(SRC_DIR)/../share/lset.h
ra_items.$(SUF):	$(SRC_DIR)/../share/global.h
ra_items.$(SUF):	$(SRC_DIR)/../share/def.h
ra_items.$(SUF):	$(SRC_DIR)/../share/debug.h
ra_items.$(SUF):	$(SRC_DIR)/../share/types.h
ra_items.$(SUF):	$(EMH)/em_reg.h
ra_items.$(SUF):	$(EMH)/em_pseu.h
ra_items.$(SUF):	$(EMH)/em_spec.h
ra_items.$(SUF):	$(EMH)/em_mnem.h
ra_lifet.$(SUF):	$(SRC_DIR)/ra_lifet.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ra_lifet.c
ra_lifet.$(SUF):	$(SRC_DIR)/ra_lifet.h
ra_lifet.$(SUF):	$(SRC_DIR)/ra_items.h
ra_lifet.$(SUF):	$(SRC_DIR)/ra_aux.h
ra_lifet.$(SUF):	$(SRC_DIR)/ra.h
ra_lifet.$(SUF):	$(SRC_DIR)/../share/alloc.h
ra_lifet.$(SUF):	$(SRC_DIR)/../share/aux.h
ra_lifet.$(SUF):	$(SRC_DIR)/../share/lset.h
ra_lifet.$(SUF):	$(SRC_DIR)/../share/global.h
ra_lifet.$(SUF):	$(SRC_DIR)/../share/def.h
ra_lifet.$(SUF):	$(SRC_DIR)/../share/debug.h
ra_lifet.$(SUF):	$(SRC_DIR)/../share/types.h
ra_lifet.$(SUF):	$(EMH)/em_ego.h
ra_lifet.$(SUF):	$(EMH)/em_mes.h
ra_lifet.$(SUF):	$(EMH)/em_reg.h
ra_lifet.$(SUF):	$(EMH)/em_pseu.h
ra_lifet.$(SUF):	$(EMH)/em_spec.h
ra_lifet.$(SUF):	$(EMH)/em_mnem.h
ra_allocl.$(SUF):	$(SRC_DIR)/ra_allocl.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ra_allocl.c
ra_allocl.$(SUF):	$(SRC_DIR)/ra_interv.h
ra_allocl.$(SUF):	$(SRC_DIR)/ra_allocl.h
ra_allocl.$(SUF):	$(SRC_DIR)/ra_items.h
ra_allocl.$(SUF):	$(SRC_DIR)/ra_aux.h
ra_allocl.$(SUF):	$(SRC_DIR)/ra.h
ra_allocl.$(SUF):	$(SRC_DIR)/../share/map.h
ra_allocl.$(SUF):	$(SRC_DIR)/../share/alloc.h
ra_allocl.$(SUF):	$(SRC_DIR)/../share/aux.h
ra_allocl.$(SUF):	$(SRC_DIR)/../share/cset.h
ra_allocl.$(SUF):	$(SRC_DIR)/../share/lset.h
ra_allocl.$(SUF):	$(SRC_DIR)/../share/global.h
ra_allocl.$(SUF):	$(SRC_DIR)/../share/def.h
ra_allocl.$(SUF):	$(SRC_DIR)/../share/debug.h
ra_allocl.$(SUF):	$(SRC_DIR)/../share/types.h
ra_allocl.$(SUF):	$(EMH)/em_reg.h
ra_allocl.$(SUF):	$(EMH)/em_pseu.h
ra_allocl.$(SUF):	$(EMH)/em_spec.h
ra_allocl.$(SUF):	$(EMH)/em_mnem.h
ra_profits.$(SUF):	$(SRC_DIR)/ra_profits.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ra_profits.c
ra_profits.$(SUF):	$(SRC_DIR)/ra_profits.h
ra_profits.$(SUF):	$(SRC_DIR)/ra_aux.h
ra_profits.$(SUF):	$(SRC_DIR)/ra.h
ra_profits.$(SUF):	$(SRC_DIR)/../share/global.h
ra_profits.$(SUF):	$(SRC_DIR)/../share/lset.h
ra_profits.$(SUF):	$(SRC_DIR)/../share/debug.h
ra_profits.$(SUF):	$(SRC_DIR)/../share/types.h
ra_profits.$(SUF):	$(EMH)/em_reg.h
ra_interv.$(SUF):	$(SRC_DIR)/ra_interv.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ra_interv.c
ra_interv.$(SUF):	$(SRC_DIR)/ra_interv.h
ra_interv.$(SUF):	$(SRC_DIR)/ra.h
ra_interv.$(SUF):	$(SRC_DIR)/../share/lset.h
ra_interv.$(SUF):	$(SRC_DIR)/../share/alloc.h
ra_interv.$(SUF):	$(SRC_DIR)/../share/global.h
ra_interv.$(SUF):	$(SRC_DIR)/../share/debug.h
ra_interv.$(SUF):	$(SRC_DIR)/../share/types.h
ra_interv.$(SUF):	$(EMH)/em_reg.h
ra_pack.$(SUF):	$(SRC_DIR)/ra_pack.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ra_pack.c
ra_pack.$(SUF):	$(SRC_DIR)/ra_interv.h
ra_pack.$(SUF):	$(SRC_DIR)/ra_aux.h
ra_pack.$(SUF):	$(SRC_DIR)/ra.h
ra_pack.$(SUF):	$(SRC_DIR)/../share/aux.h
ra_pack.$(SUF):	$(SRC_DIR)/../share/alloc.h
ra_pack.$(SUF):	$(SRC_DIR)/../share/cset.h
ra_pack.$(SUF):	$(SRC_DIR)/../share/lset.h
ra_pack.$(SUF):	$(SRC_DIR)/../share/global.h
ra_pack.$(SUF):	$(SRC_DIR)/../share/def.h
ra_pack.$(SUF):	$(SRC_DIR)/../share/debug.h
ra_pack.$(SUF):	$(SRC_DIR)/../share/types.h
ra_pack.$(SUF):	$(EMH)/em_reg.h
ra_xform.$(SUF):	$(SRC_DIR)/ra_xform.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ra_xform.c
ra_xform.$(SUF):	$(SRC_DIR)/ra_items.h
ra_xform.$(SUF):	$(SRC_DIR)/ra_xform.h
ra_xform.$(SUF):	$(SRC_DIR)/ra_interv.h
ra_xform.$(SUF):	$(SRC_DIR)/ra.h
ra_xform.$(SUF):	$(SRC_DIR)/../share/alloc.h
ra_xform.$(SUF):	$(SRC_DIR)/../share/aux.h
ra_xform.$(SUF):	$(SRC_DIR)/../share/lset.h
ra_xform.$(SUF):	$(SRC_DIR)/../share/global.h
ra_xform.$(SUF):	$(SRC_DIR)/../share/def.h
ra_xform.$(SUF):	$(SRC_DIR)/../share/debug.h
ra_xform.$(SUF):	$(SRC_DIR)/../share/types.h
ra_xform.$(SUF):	$(EMH)/em_reg.h
ra_xform.$(SUF):	$(EMH)/em_ego.h
ra_xform.$(SUF):	$(EMH)/em_mes.h
ra_xform.$(SUF):	$(EMH)/em_pseu.h
ra_xform.$(SUF):	$(EMH)/em_spec.h
ra_xform.$(SUF):	$(EMH)/em_mnem.h
ra_aux.$(SUF):	$(SRC_DIR)/ra_aux.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ra_aux.c
ra_aux.$(SUF):	$(SRC_DIR)/ra_aux.h
ra_aux.$(SUF):	$(SRC_DIR)/ra.h
ra_aux.$(SUF):	$(SRC_DIR)/../share/alloc.h
ra_aux.$(SUF):	$(SRC_DIR)/../share/lset.h
ra_aux.$(SUF):	$(SRC_DIR)/../share/global.h
ra_aux.$(SUF):	$(SRC_DIR)/../share/def.h
ra_aux.$(SUF):	$(SRC_DIR)/../share/debug.h
ra_aux.$(SUF):	$(SRC_DIR)/../share/types.h
ra_aux.$(SUF):	$(EMH)/em_reg.h
ra_aux.$(SUF):	$(EMH)/em_pseu.h
ra_aux.$(SUF):	$(EMH)/em_spec.h
ra_aux.$(SUF):	$(EMH)/em_mnem.h
