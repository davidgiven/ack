# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/ego/sr
EMH=$(TARGET_HOME)/h
EMLIB=$(TARGET_HOME)/lib.bin

LDFLAGS=$(LDOPTIONS)
CPPFLAGS=-DVERBOSE -DNOTCOMPACT -I$(EMH) -I$(SRC_DIR) -I$(EMLIB)/ego
CFLAGS=$(CPPFLAGS) $(COPTIONS)
LINTFLAGS=$(CPPFLAGS) $(LINTOPTIONS)

CFILES=\
	$(SRC_DIR)/sr.c \
	$(SRC_DIR)/sr_aux.c \
	$(SRC_DIR)/sr_cand.c \
	$(SRC_DIR)/sr_expr.c \
	$(SRC_DIR)/sr_iv.c \
	$(SRC_DIR)/sr_reduce.c \
	$(SRC_DIR)/sr_xform.c

OFILES=\
	sr.$(SUF) sr_aux.$(SUF) sr_cand.$(SUF) sr_expr.$(SUF) sr_iv.$(SUF) \
	sr_reduce.$(SUF) sr_xform.$(SUF)

HFILES=\
	$(SRC_DIR)/sr.h \
	$(SRC_DIR)/sr_aux.h \
	$(SRC_DIR)/sr_cand.h \
	$(SRC_DIR)/sr_expr.h \
	$(SRC_DIR)/sr_iv.h \
	$(SRC_DIR)/sr_reduce.h \
	$(SRC_DIR)/sr_xform.h

PRFILES=\
	$(CFILES) $(HFILES) $(SRC_DIR)/proto.make

all:	sr

sr:	$(OFILES)
	$(CC) -o sr $(LDFLAGS) $(OFILES) $(EMLIB)/ego/share.$(LIBSUF) $(EMLIB)/em_data.$(LIBSUF)

install:	all
	cp sr $(EMLIB)/ego/sr

cmp:	all
	-cmp sr $(EMLIB)/ego/sr

clean:
	rm -f *.$(SUF) sr Out out nohup.out

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
sr.$(SUF):	$(SRC_DIR)/sr.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/sr.c
sr.$(SUF):	$(SRC_DIR)/sr_iv.h
sr.$(SUF):	$(SRC_DIR)/sr_aux.h
sr.$(SUF):	$(SRC_DIR)/../share/aux.h
sr.$(SUF):	$(SRC_DIR)/../share/go.h
sr.$(SUF):	$(SRC_DIR)/../share/alloc.h
sr.$(SUF):	$(SRC_DIR)/../share/map.h
sr.$(SUF):	$(SRC_DIR)/../share/lset.h
sr.$(SUF):	$(SRC_DIR)/../share/put.h
sr.$(SUF):	$(SRC_DIR)/../share/get.h
sr.$(SUF):	$(SRC_DIR)/../share/files.h
sr.$(SUF):	$(SRC_DIR)/../share/global.h
sr.$(SUF):	$(SRC_DIR)/../share/debug.h
sr.$(SUF):	$(SRC_DIR)/sr.h
sr.$(SUF):	$(SRC_DIR)/../share/types.h
sr_aux.$(SUF):	$(SRC_DIR)/sr_aux.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/sr_aux.c
sr_aux.$(SUF):	$(SRC_DIR)/sr_xform.h
sr_aux.$(SUF):	$(SRC_DIR)/sr_aux.h
sr_aux.$(SUF):	$(SRC_DIR)/../share/aux.h
sr_aux.$(SUF):	$(SRC_DIR)/../share/lset.h
sr_aux.$(SUF):	$(SRC_DIR)/../share/global.h
sr_aux.$(SUF):	$(SRC_DIR)/../share/debug.h
sr_aux.$(SUF):	$(SRC_DIR)/sr.h
sr_aux.$(SUF):	$(SRC_DIR)/../share/types.h
sr_aux.$(SUF):	$(EMH)/em_pseu.h
sr_aux.$(SUF):	$(EMH)/em_mnem.h
sr_cand.$(SUF):	$(SRC_DIR)/sr_cand.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/sr_cand.c
sr_cand.$(SUF):	$(SRC_DIR)/sr_cand.h
sr_cand.$(SUF):	$(SRC_DIR)/sr_aux.h
sr_cand.$(SUF):	$(SRC_DIR)/sr.h
sr_cand.$(SUF):	$(SRC_DIR)/../share/aux.h
sr_cand.$(SUF):	$(SRC_DIR)/../share/map.h
sr_cand.$(SUF):	$(SRC_DIR)/../share/global.h
sr_cand.$(SUF):	$(SRC_DIR)/../share/debug.h
sr_cand.$(SUF):	$(SRC_DIR)/../share/cset.h
sr_cand.$(SUF):	$(SRC_DIR)/../share/lset.h
sr_cand.$(SUF):	$(SRC_DIR)/../share/types.h
sr_cand.$(SUF):	$(EMH)/em_pseu.h
sr_cand.$(SUF):	$(EMH)/em_mnem.h
sr_expr.$(SUF):	$(SRC_DIR)/sr_expr.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/sr_expr.c
sr_expr.$(SUF):	$(SRC_DIR)/sr_iv.h
sr_expr.$(SUF):	$(SRC_DIR)/../share/lset.h
sr_expr.$(SUF):	$(SRC_DIR)/sr_aux.h
sr_expr.$(SUF):	$(SRC_DIR)/../share/aux.h
sr_expr.$(SUF):	$(SRC_DIR)/../share/global.h
sr_expr.$(SUF):	$(SRC_DIR)/../share/debug.h
sr_expr.$(SUF):	$(SRC_DIR)/sr.h
sr_expr.$(SUF):	$(SRC_DIR)/../share/types.h
sr_expr.$(SUF):	$(EMH)/em_mnem.h
sr_iv.$(SUF):	$(SRC_DIR)/sr_iv.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/sr_iv.c
sr_iv.$(SUF):	$(SRC_DIR)/sr_iv.h
sr_iv.$(SUF):	$(SRC_DIR)/sr_cand.h
sr_iv.$(SUF):	$(SRC_DIR)/sr_aux.h
sr_iv.$(SUF):	$(SRC_DIR)/../share/aux.h
sr_iv.$(SUF):	$(SRC_DIR)/../share/alloc.h
sr_iv.$(SUF):	$(SRC_DIR)/../share/global.h
sr_iv.$(SUF):	$(SRC_DIR)/../share/debug.h
sr_iv.$(SUF):	$(SRC_DIR)/../share/cset.h
sr_iv.$(SUF):	$(SRC_DIR)/../share/lset.h
sr_iv.$(SUF):	$(SRC_DIR)/sr.h
sr_iv.$(SUF):	$(SRC_DIR)/../share/types.h
sr_iv.$(SUF):	$(EMH)/em_pseu.h
sr_iv.$(SUF):	$(EMH)/em_mnem.h
sr_reduce.$(SUF):	$(SRC_DIR)/sr_reduce.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/sr_reduce.c
sr_reduce.$(SUF):	$(SRC_DIR)/sr_expr.h
sr_reduce.$(SUF):	$(SRC_DIR)/sr_reduce.h
sr_reduce.$(SUF):	$(SRC_DIR)/sr_xform.h
sr_reduce.$(SUF):	$(SRC_DIR)/../share/lset.h
sr_reduce.$(SUF):	$(SRC_DIR)/sr_aux.h
sr_reduce.$(SUF):	$(SRC_DIR)/../share/aux.h
sr_reduce.$(SUF):	$(SRC_DIR)/../share/global.h
sr_reduce.$(SUF):	$(SRC_DIR)/../share/def.h
sr_reduce.$(SUF):	$(SRC_DIR)/../share/alloc.h
sr_reduce.$(SUF):	$(SRC_DIR)/../share/debug.h
sr_reduce.$(SUF):	$(SRC_DIR)/sr.h
sr_reduce.$(SUF):	$(SRC_DIR)/../share/types.h
sr_reduce.$(SUF):	$(EMH)/em_spec.h
sr_reduce.$(SUF):	$(EMH)/em_mnem.h
sr_reduce.$(SUF):	$(EMH)/em_mes.h
sr_reduce.$(SUF):	$(EMH)/em_reg.h
sr_reduce.$(SUF):	$(EMH)/em_pseu.h
sr_xform.$(SUF):	$(SRC_DIR)/sr_xform.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/sr_xform.c
sr_xform.$(SUF):	$(SRC_DIR)/sr_xform.h
sr_xform.$(SUF):	$(SRC_DIR)/../share/aux.h
sr_xform.$(SUF):	$(SRC_DIR)/../share/lset.h
sr_xform.$(SUF):	$(SRC_DIR)/sr_aux.h
sr_xform.$(SUF):	$(SRC_DIR)/../share/get.h
sr_xform.$(SUF):	$(SRC_DIR)/../share/def.h
sr_xform.$(SUF):	$(SRC_DIR)/../share/alloc.h
sr_xform.$(SUF):	$(SRC_DIR)/../share/global.h
sr_xform.$(SUF):	$(SRC_DIR)/../share/debug.h
sr_xform.$(SUF):	$(SRC_DIR)/sr.h
sr_xform.$(SUF):	$(SRC_DIR)/../share/types.h
sr_xform.$(SUF):	$(EMH)/em_spec.h
sr_xform.$(SUF):	$(EMH)/em_pseu.h
sr_xform.$(SUF):	$(EMH)/em_mnem.h
