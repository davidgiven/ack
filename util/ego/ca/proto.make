# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/ego/ca
EMH=$(TARGET_HOME)/h
EMLIB=$(TARGET_HOME)/lib.bin

LDFLAGS=$(LDOPTIONS)
CPPFLAGS=-DVERBOSE -DNOTCOMPACT -I$(EMH) -I$(SRC_DIR) -I$(EMLIB)/ego
CFLAGS=$(CPPFLAGS) $(COPTIONS)
LINTFLAGS=$(CPPFLAGS) $(LINTOPTIONS)

CFILES=\
	$(SRC_DIR)/ca.c \
	$(SRC_DIR)/ca_put.c

OFILES=\
ca.$(SUF) ca_put.$(SUF)

HFILES=\
	$(SRC_DIR)/ca.h \
	$(SRC_DIR)/ca_put.h

PRFILES=\
	$(CFILES) $(HFILES) $(SRC_DIR)/proto.make

all:	ca

ca:	$(OFILES)
	$(CC) -o ca $(LDFLAGS) $(OFILES) $(EMLIB)/ego/share.$(LIBSUF) $(EMLIB)/em_data.$(LIBSUF)

install:	all
	cp ca $(EMLIB)/ego/ca

cmp:	all
	-cmp ca $(EMLIB)/ego/ca

clean:
	rm -f *.$(SUF) ca Out out nohup.out

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
ca.$(SUF):	$(SRC_DIR)/ca.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ca.c
ca.$(SUF):	$(SRC_DIR)/ca_put.h
ca.$(SUF):	$(SRC_DIR)/../share/get.h
ca.$(SUF):	$(SRC_DIR)/../share/alloc.h
ca.$(SUF):	$(SRC_DIR)/../share/map.h
ca.$(SUF):	$(SRC_DIR)/../share/files.h
ca.$(SUF):	$(SRC_DIR)/../share/lset.h
ca.$(SUF):	$(SRC_DIR)/../share/global.h
ca.$(SUF):	$(SRC_DIR)/../share/debug.h
ca.$(SUF):	$(SRC_DIR)/ca.h
ca.$(SUF):	$(SRC_DIR)/../share/types.h
ca.$(SUF):	$(EMH)/em_mes.h
ca.$(SUF):	$(EMH)/em_pseu.h
ca_put.$(SUF):	$(SRC_DIR)/ca_put.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/ca_put.c
ca_put.$(SUF):	$(SRC_DIR)/../share/alloc.h
ca_put.$(SUF):	$(SRC_DIR)/../share/map.h
ca_put.$(SUF):	$(SRC_DIR)/../share/def.h
ca_put.$(SUF):	$(SRC_DIR)/../share/debug.h
ca_put.$(SUF):	$(SRC_DIR)/ca.h
ca_put.$(SUF):	$(SRC_DIR)/../share/types.h
ca_put.$(SUF):	$(EMH)/em_mes.h
ca_put.$(SUF):	$(EMH)/em_flag.h
ca_put.$(SUF):	$(EMH)/em_mnem.h
ca_put.$(SUF):	$(EMH)/em_pseu.h
ca_put.$(SUF):	$(EMH)/em_spec.h
