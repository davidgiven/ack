# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/ego/il
EMH=$(TARGET_HOME)/h
EMCONFIG=$(TARGET_HOME)/config
EMLIB=$(TARGET_HOME)/lib.bin

LDFLAGS=$(LDOPTIONS)
CPPFLAGS=-DVERBOSE -DNOTCOMPACT -I$(EMH) -I$(EMCONFIG) -I$(SRC_DIR) -I$(EMLIB)/ego
CFLAGS=$(CPPFLAGS) $(COPTIONS)
LINTFLAGS=$(CPPFLAGS) $(LINTOPTIONS)

CFILES=\
	$(SRC_DIR)/il.c \
	$(SRC_DIR)/il1_anal.c \
	$(SRC_DIR)/il1_cal.c \
	$(SRC_DIR)/il1_formal.c \
	$(SRC_DIR)/il1_aux.c \
	$(SRC_DIR)/il2_aux.c \
	$(SRC_DIR)/il3_change.c \
	$(SRC_DIR)/il3_subst.c \
	$(SRC_DIR)/il3_aux.c \
	$(SRC_DIR)/il_aux.c

OFILES=\
	il.$(SUF) il1_anal.$(SUF) il1_cal.$(SUF) il1_formal.$(SUF) \
	il1_aux.$(SUF) il2_aux.$(SUF) il3_change.$(SUF) il3_subst.$(SUF) \
	il3_aux.$(SUF) il_aux.$(SUF)

HFILES=\
	$(SRC_DIR)/il.h \
	$(SRC_DIR)/il1_anal.h \
	$(SRC_DIR)/il1_cal.h \
	$(SRC_DIR)/il1_formal.h \
	$(SRC_DIR)/il1_aux.h \
	$(SRC_DIR)/il2_aux.h \
	$(SRC_DIR)/il3_change.h \
	$(SRC_DIR)/il3_subst.h \
	$(SRC_DIR)/il3_aux.h \
	$(SRC_DIR)/il_aux.h

PRFILES=\
	$(CFILES) $(HFILES) $(SRC_DIR)/proto.make

all:	il

il:	$(OFILES)
	$(CC) -o il $(LDFLAGS) $(OFILES) $(EMLIB)/ego/share.$(LIBSUF) $(EMLIB)/em_data.$(LIBSUF)

install:	all
	cp il $(EMLIB)/ego/il

cmp:	all
	-cmp il $(EMLIB)/ego/il

clean:
	rm -f *.$(SUF) il Out out nohup.out

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
il.$(SUF):	$(SRC_DIR)/il.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/il.c
il.$(SUF):	$(SRC_DIR)/../share/go.h
il.$(SUF):	$(SRC_DIR)/../share/put.h
il.$(SUF):	$(SRC_DIR)/../share/get.h
il.$(SUF):	$(SRC_DIR)/il3_subst.h
il.$(SUF):	$(SRC_DIR)/il2_aux.h
il.$(SUF):	$(SRC_DIR)/il1_anal.h
il.$(SUF):	$(SRC_DIR)/il_aux.h
il.$(SUF):	$(SRC_DIR)/../share/map.h
il.$(SUF):	$(SRC_DIR)/../share/files.h
il.$(SUF):	$(SRC_DIR)/../share/lset.h
il.$(SUF):	$(SRC_DIR)/../share/global.h
il.$(SUF):	$(SRC_DIR)/../share/alloc.h
il.$(SUF):	$(SRC_DIR)/../share/debug.h
il.$(SUF):	$(SRC_DIR)/il.h
il.$(SUF):	$(SRC_DIR)/../share/types.h
il.$(SUF):	$(EMH)/em_pseu.h
il.$(SUF):	$(EMH)/em_mnem.h
il.$(SUF):	$(EMCONFIG)/em_path.h
il1_anal.$(SUF):	$(SRC_DIR)/il1_anal.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/il1_anal.c
il1_anal.$(SUF):	$(SRC_DIR)/../share/put.h
il1_anal.$(SUF):	$(SRC_DIR)/il_aux.h
il1_anal.$(SUF):	$(SRC_DIR)/il1_anal.h
il1_anal.$(SUF):	$(SRC_DIR)/il1_cal.h
il1_anal.$(SUF):	$(SRC_DIR)/il1_formal.h
il1_anal.$(SUF):	$(SRC_DIR)/il1_aux.h
il1_anal.$(SUF):	$(SRC_DIR)/../share/aux.h
il1_anal.$(SUF):	$(SRC_DIR)/../share/lset.h
il1_anal.$(SUF):	$(SRC_DIR)/../share/global.h
il1_anal.$(SUF):	$(SRC_DIR)/../share/alloc.h
il1_anal.$(SUF):	$(SRC_DIR)/../share/debug.h
il1_anal.$(SUF):	$(SRC_DIR)/il.h
il1_anal.$(SUF):	$(SRC_DIR)/../share/types.h
il1_anal.$(SUF):	$(EMH)/em_pseu.h
il1_anal.$(SUF):	$(EMH)/em_mnem.h
il1_cal.$(SUF):	$(SRC_DIR)/il1_cal.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/il1_cal.c
il1_cal.$(SUF):	$(SRC_DIR)/../share/parser.h
il1_cal.$(SUF):	$(SRC_DIR)/il1_aux.h
il1_cal.$(SUF):	$(SRC_DIR)/../share/lset.h
il1_cal.$(SUF):	$(SRC_DIR)/../share/global.h
il1_cal.$(SUF):	$(SRC_DIR)/../share/alloc.h
il1_cal.$(SUF):	$(SRC_DIR)/../share/debug.h
il1_cal.$(SUF):	$(SRC_DIR)/il1_cal.h
il1_cal.$(SUF):	$(SRC_DIR)/il.h
il1_cal.$(SUF):	$(SRC_DIR)/../share/types.h
il1_cal.$(SUF):	$(EMH)/em_mnem.h
il1_cal.$(SUF):	$(EMH)/em_spec.h
il1_formal.$(SUF):	$(SRC_DIR)/il1_formal.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/il1_formal.c
il1_formal.$(SUF):	$(SRC_DIR)/il1_formal.h
il1_formal.$(SUF):	$(SRC_DIR)/il1_aux.h
il1_formal.$(SUF):	$(SRC_DIR)/../share/lset.h
il1_formal.$(SUF):	$(SRC_DIR)/../share/global.h
il1_formal.$(SUF):	$(SRC_DIR)/../share/alloc.h
il1_formal.$(SUF):	$(SRC_DIR)/../share/debug.h
il1_formal.$(SUF):	$(SRC_DIR)/il.h
il1_formal.$(SUF):	$(SRC_DIR)/../share/types.h
il1_aux.$(SUF):	$(SRC_DIR)/il1_aux.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/il1_aux.c
il1_aux.$(SUF):	$(SRC_DIR)/il1_aux.h
il1_aux.$(SUF):	$(SRC_DIR)/il_aux.h
il1_aux.$(SUF):	$(SRC_DIR)/../share/lset.h
il1_aux.$(SUF):	$(SRC_DIR)/../share/global.h
il1_aux.$(SUF):	$(SRC_DIR)/../share/alloc.h
il1_aux.$(SUF):	$(SRC_DIR)/../share/debug.h
il1_aux.$(SUF):	$(SRC_DIR)/il.h
il1_aux.$(SUF):	$(SRC_DIR)/../share/types.h
il1_aux.$(SUF):	$(EMH)/em_spec.h
il2_aux.$(SUF):	$(SRC_DIR)/il2_aux.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/il2_aux.c
il2_aux.$(SUF):	$(SRC_DIR)/../share/aux.h
il2_aux.$(SUF):	$(SRC_DIR)/../share/get.h
il2_aux.$(SUF):	$(SRC_DIR)/il2_aux.h
il2_aux.$(SUF):	$(SRC_DIR)/il_aux.h
il2_aux.$(SUF):	$(SRC_DIR)/../share/lset.h
il2_aux.$(SUF):	$(SRC_DIR)/../share/global.h
il2_aux.$(SUF):	$(SRC_DIR)/../share/alloc.h
il2_aux.$(SUF):	$(SRC_DIR)/../share/debug.h
il2_aux.$(SUF):	$(SRC_DIR)/il.h
il2_aux.$(SUF):	$(SRC_DIR)/../share/types.h
il2_aux.$(SUF):	$(EMH)/em_mnem.h
il2_aux.$(SUF):	$(EMH)/em_spec.h
il3_change.$(SUF):	$(SRC_DIR)/il3_change.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/il3_change.c
il3_change.$(SUF):	$(SRC_DIR)/il3_aux.h
il3_change.$(SUF):	$(SRC_DIR)/il3_change.h
il3_change.$(SUF):	$(SRC_DIR)/il_aux.h
il3_change.$(SUF):	$(SRC_DIR)/../share/put.h
il3_change.$(SUF):	$(SRC_DIR)/../share/get.h
il3_change.$(SUF):	$(SRC_DIR)/../share/aux.h
il3_change.$(SUF):	$(SRC_DIR)/../share/lset.h
il3_change.$(SUF):	$(SRC_DIR)/../share/def.h
il3_change.$(SUF):	$(SRC_DIR)/../share/global.h
il3_change.$(SUF):	$(SRC_DIR)/../share/alloc.h
il3_change.$(SUF):	$(SRC_DIR)/../share/debug.h
il3_change.$(SUF):	$(SRC_DIR)/il.h
il3_change.$(SUF):	$(SRC_DIR)/../share/types.h
il3_change.$(SUF):	$(EMH)/em_mes.h
il3_change.$(SUF):	$(EMH)/em_spec.h
il3_change.$(SUF):	$(EMH)/em_pseu.h
il3_change.$(SUF):	$(EMH)/em_mnem.h
il3_subst.$(SUF):	$(SRC_DIR)/il3_subst.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/il3_subst.c
il3_subst.$(SUF):	$(SRC_DIR)/il3_subst.h
il3_subst.$(SUF):	$(SRC_DIR)/il3_change.h
il3_subst.$(SUF):	$(SRC_DIR)/il3_aux.h
il3_subst.$(SUF):	$(SRC_DIR)/il_aux.h
il3_subst.$(SUF):	$(SRC_DIR)/../share/get.h
il3_subst.$(SUF):	$(SRC_DIR)/../share/lset.h
il3_subst.$(SUF):	$(SRC_DIR)/../share/global.h
il3_subst.$(SUF):	$(SRC_DIR)/../share/alloc.h
il3_subst.$(SUF):	$(SRC_DIR)/../share/debug.h
il3_subst.$(SUF):	$(SRC_DIR)/il.h
il3_subst.$(SUF):	$(SRC_DIR)/../share/types.h
il3_subst.$(SUF):	$(EMH)/em_mnem.h
il3_aux.$(SUF):	$(SRC_DIR)/il3_aux.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/il3_aux.c
il3_aux.$(SUF):	$(SRC_DIR)/il3_aux.h
il3_aux.$(SUF):	$(SRC_DIR)/il_aux.h
il3_aux.$(SUF):	$(SRC_DIR)/../share/global.h
il3_aux.$(SUF):	$(SRC_DIR)/../share/alloc.h
il3_aux.$(SUF):	$(SRC_DIR)/../share/debug.h
il3_aux.$(SUF):	$(SRC_DIR)/il.h
il3_aux.$(SUF):	$(SRC_DIR)/../share/types.h
il_aux.$(SUF):	$(SRC_DIR)/il_aux.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/il_aux.c
il_aux.$(SUF):	$(SRC_DIR)/il_aux.h
il_aux.$(SUF):	$(SRC_DIR)/../share/map.h
il_aux.$(SUF):	$(SRC_DIR)/../share/lset.h
il_aux.$(SUF):	$(SRC_DIR)/../share/global.h
il_aux.$(SUF):	$(SRC_DIR)/../share/alloc.h
il_aux.$(SUF):	$(SRC_DIR)/../share/put.h
il_aux.$(SUF):	$(SRC_DIR)/../share/get.h
il_aux.$(SUF):	$(SRC_DIR)/../share/debug.h
il_aux.$(SUF):	$(SRC_DIR)/il.h
il_aux.$(SUF):	$(SRC_DIR)/../share/types.h
il_aux.$(SUF):	$(EMH)/em_pseu.h
il_aux.$(SUF):	$(EMH)/em_mnem.h
il_aux.$(SUF):	$(EMH)/em_spec.h
