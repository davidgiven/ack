# $Header$

#PARAMS		do not remove this line

SRC_DIR = $(SRC_HOME)/util/ego/share
EMH=$(TARGET_HOME)/h

LDFLAGS=$(LDOPTIONS)
CPPFLAGS=-DVERBOSE -DNOTCOMPACT -I$(EMH) -I$(SRC_DIR) -I.
CFLAGS=$(CPPFLAGS) $(COPTIONS)
UCFLAGS=$(CPPFLAGS) $(UCOPTIONS)
ULDFLAGS=$(ULDOPTIONS)

CFILES=\
	$(SRC_DIR)/debug.c \
	$(SRC_DIR)/global.c \
	$(SRC_DIR)/files.c \
	$(SRC_DIR)/go.c \
	$(SRC_DIR)/map.c \
	$(SRC_DIR)/aux.c \
	$(SRC_DIR)/get.c \
	$(SRC_DIR)/put.c \
	$(SRC_DIR)/alloc.c \
	$(SRC_DIR)/lset.c \
	$(SRC_DIR)/cset.c \
	$(SRC_DIR)/parser.c \
	$(SRC_DIR)/stack_chg.c \
	$(SRC_DIR)/locals.c \
	$(SRC_DIR)/init_glob.c
SRC=\
	$(SRC_DIR)/types.h \
	$(SRC_DIR)/def.h \
	$(SRC_DIR)/debug.h \
	$(SRC_DIR)/global.h \
	$(SRC_DIR)/files.h \
	$(SRC_DIR)/go.h \
	$(SRC_DIR)/map.h \
	$(SRC_DIR)/aux.h \
	$(SRC_DIR)/get.h \
	$(SRC_DIR)/put.h \
	$(SRC_DIR)/alloc.h\
	$(SRC_DIR)/lset.h \
	$(SRC_DIR)/cset.h \
	$(SRC_DIR)/parser.h \
	$(SRC_DIR)/stack_chg.h \
	$(SRC_DIR)/locals.h \
	$(SRC_DIR)/init_glob.h \
	$(CFILES)

PRFILES=$(SRC)

OBS =	alloc.$(SUF) cset.$(SUF) debug.$(SUF) \
	files.$(SUF) go.$(SUF) global.$(SUF) lset.$(SUF) map.$(SUF) \
	parser.$(SUF) get.$(SUF) put.$(SUF) aux.$(SUF) stack_chg.$(SUF) \
	locals.$(SUF) init_glob.$(SUF)

all:		classdefs.h pop_push.h $(OBS)
		$(AR) r share.$(LIBSUF) $(OBS)
		$(RANLIB) share.$(LIBSUF)

install:	all
		-mkdir $(TARGET_HOME)/lib.bin/ego
		cp share.$(LIBSUF) $(TARGET_HOME)/lib.bin/ego/share.$(LIBSUF)
		$(RANLIB) $(TARGET_HOME)/lib.bin/ego/share.$(LIBSUF)
		cp classdefs.h $(TARGET_HOME)/lib.bin/ego/classdefs.h
		cp pop_push.h $(TARGET_HOME)/lib.bin/ego/pop_push.h

cmp:		all
		-cmp share.$(LIBSUF) $(TARGET_HOME)/lib.bin/ego/share.$(LIBSUF)
		-cmp classdefs.h $(TARGET_HOME)/lib.bin/ego/classdefs.h
		-cmp pop_push.h $(TARGET_HOME)/lib.bin/ego/pop_push.h

classdefs.h: \
	makeclassdef \
	$(SRC_DIR)/cldefs.src
	 makeclassdef $(EMH)/em_mnem.h $(SRC_DIR)/cldefs.src > classdefs.h

makeclassdef: \
	$(SRC_DIR)/makecldef.c
	 $(UCC) $(UCFLAGS) $(ULDFLAGS) -o makeclassdef $(SRC_DIR)/makecldef.c

pop_push.h: \
	$(SRC_HOME)/etc/em_table $(SRC_DIR)/pop_push.awk
	 awk -f $(SRC_DIR)/pop_push.awk < $(SRC_HOME)/etc/em_table > pop_push.h

show: \
	$(SRC_DIR)/show.c
	 $(UCC) $(UCFLAGS) $(ULDFLAGS) -o show show.c $(UTIL_HOME)/lib.bin/em_data.$(ULIBSUF)

pr:
	@pr $(PRFILES)

opr:
	make pr | opr

clean:
	rm -f makeclassdef classdefs.h *.$(SUF) Out out nohup.out

lintlib:
	-mkdir $(TARGET_HOME)/lib.bin/ego
	$(MK_LINT_LIB) share $(TARGET_HOME)/lib.bin/ego $(CPPFLAGS) $(CFILES)

depend:	pop_push.h classdefs.h
	sed '/^#DEPENDENCIES/,$$d' Makefile >Makefile.new
	echo '#DEPENDENCIES' >>Makefile.new
	for i in $(CFILES) ; do \
		echo "`basename $$i .c`.$$(SUF):	$$i" >> Makefile.new ; \
		echo '	$$(CC) -c $$(CFLAGS)' $$i >> Makefile.new ; \
		$(UTIL_HOME)/lib.bin/cpp -d $(CPPFLAGS) $$i | sed "s/^/`basename $$i .c`.$$(SUF):	/" >> Makefile.new ; \
	done
	mv Makefile Makefile.old
	mv Makefile.new Makefile

# do not remove the next line.
#DEPENDENCIES
debug.$(SUF):	$(SRC_DIR)/debug.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/debug.c
debug.$(SUF):	$(SRC_DIR)/global.h
debug.$(SUF):	$(SRC_DIR)/debug.h
debug.$(SUF):	$(SRC_DIR)/def.h
debug.$(SUF):	$(SRC_DIR)/types.h
debug.$(SUF):	$(EMH)/em_spec.h
global.$(SUF):	$(SRC_DIR)/global.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/global.c
global.$(SUF):	$(SRC_DIR)/types.h
files.$(SUF):	$(SRC_DIR)/files.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/files.c
go.$(SUF):	$(SRC_DIR)/go.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/go.c
go.$(SUF):	$(SRC_DIR)/go.h
go.$(SUF):	$(SRC_DIR)/alloc.h
go.$(SUF):	$(SRC_DIR)/map.h
go.$(SUF):	$(SRC_DIR)/lset.h
go.$(SUF):	$(SRC_DIR)/put.h
go.$(SUF):	$(SRC_DIR)/get.h
go.$(SUF):	$(SRC_DIR)/files.h
go.$(SUF):	$(SRC_DIR)/global.h
go.$(SUF):	$(SRC_DIR)/debug.h
go.$(SUF):	$(SRC_DIR)/types.h
map.$(SUF):	$(SRC_DIR)/map.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/map.c
map.$(SUF):	$(SRC_DIR)/map.h
map.$(SUF):	$(SRC_DIR)/types.h
aux.$(SUF):	$(SRC_DIR)/aux.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/aux.c
aux.$(SUF):	$(SRC_DIR)/lset.h
aux.$(SUF):	$(SRC_DIR)/map.h
aux.$(SUF):	$(SRC_DIR)/aux.h
aux.$(SUF):	$(SRC_DIR)/alloc.h
aux.$(SUF):	$(SRC_DIR)/global.h
aux.$(SUF):	$(SRC_DIR)/debug.h
aux.$(SUF):	$(SRC_DIR)/types.h
aux.$(SUF):	$(EMH)/em_pseu.h
aux.$(SUF):	$(EMH)/em_mes.h
get.$(SUF):	$(SRC_DIR)/get.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/get.c
get.$(SUF):	$(SRC_DIR)/aux.h
get.$(SUF):	$(SRC_DIR)/map.h
get.$(SUF):	$(SRC_DIR)/alloc.h
get.$(SUF):	$(SRC_DIR)/get.h
get.$(SUF):	$(SRC_DIR)/cset.h
get.$(SUF):	$(SRC_DIR)/lset.h
get.$(SUF):	$(SRC_DIR)/global.h
get.$(SUF):	$(SRC_DIR)/debug.h
get.$(SUF):	$(SRC_DIR)/def.h
get.$(SUF):	$(SRC_DIR)/types.h
get.$(SUF):	$(EMH)/em_mes.h
get.$(SUF):	$(EMH)/em_pseu.h
get.$(SUF):	$(EMH)/em_mnem.h
get.$(SUF):	$(EMH)/em_spec.h
put.$(SUF):	$(SRC_DIR)/put.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/put.c
put.$(SUF):	$(SRC_DIR)/put.h
put.$(SUF):	$(SRC_DIR)/alloc.h
put.$(SUF):	$(SRC_DIR)/lset.h
put.$(SUF):	$(SRC_DIR)/map.h
put.$(SUF):	$(SRC_DIR)/def.h
put.$(SUF):	$(SRC_DIR)/debug.h
put.$(SUF):	$(SRC_DIR)/global.h
put.$(SUF):	$(SRC_DIR)/types.h
put.$(SUF):	$(EMH)/em_spec.h
put.$(SUF):	$(EMH)/em_pseu.h
alloc.$(SUF):	$(SRC_DIR)/alloc.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/alloc.c
alloc.$(SUF):	$(SRC_DIR)/alloc.h
alloc.$(SUF):	$(SRC_DIR)/debug.h
alloc.$(SUF):	$(SRC_DIR)/types.h
lset.$(SUF):	$(SRC_DIR)/lset.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/lset.c
lset.$(SUF):	$(SRC_DIR)/debug.h
lset.$(SUF):	$(SRC_DIR)/alloc.h
lset.$(SUF):	$(SRC_DIR)/lset.h
lset.$(SUF):	$(SRC_DIR)/types.h
cset.$(SUF):	$(SRC_DIR)/cset.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/cset.c
cset.$(SUF):	$(SRC_DIR)/global.h
cset.$(SUF):	$(SRC_DIR)/debug.h
cset.$(SUF):	$(SRC_DIR)/alloc.h
cset.$(SUF):	$(SRC_DIR)/cset.h
cset.$(SUF):	$(SRC_DIR)/types.h
parser.$(SUF):	$(SRC_DIR)/parser.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/parser.c
parser.$(SUF):	./classdefs.h
parser.$(SUF):	$(SRC_DIR)/aux.h
parser.$(SUF):	$(SRC_DIR)/lset.h
parser.$(SUF):	$(SRC_DIR)/global.h
parser.$(SUF):	$(SRC_DIR)/alloc.h
parser.$(SUF):	$(SRC_DIR)/debug.h
parser.$(SUF):	$(SRC_DIR)/types.h
parser.$(SUF):	$(EMH)/em_mnem.h
parser.$(SUF):	$(EMH)/em_spec.h
stack_chg.$(SUF):	$(SRC_DIR)/stack_chg.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/stack_chg.c
stack_chg.$(SUF):	./pop_push.h
stack_chg.$(SUF):	$(SRC_DIR)/global.h
stack_chg.$(SUF):	$(SRC_DIR)/debug.h
stack_chg.$(SUF):	$(SRC_DIR)/types.h
stack_chg.$(SUF):	$(EMH)/em_mnem.h
stack_chg.$(SUF):	$(EMH)/em_spec.h
locals.$(SUF):	$(SRC_DIR)/locals.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/locals.c
locals.$(SUF):	$(SRC_DIR)/locals.h
locals.$(SUF):	$(SRC_DIR)/alloc.h
locals.$(SUF):	$(SRC_DIR)/aux.h
locals.$(SUF):	$(SRC_DIR)/get.h
locals.$(SUF):	$(SRC_DIR)/def.h
locals.$(SUF):	$(SRC_DIR)/cset.h
locals.$(SUF):	$(SRC_DIR)/lset.h
locals.$(SUF):	$(SRC_DIR)/global.h
locals.$(SUF):	$(SRC_DIR)/debug.h
locals.$(SUF):	$(SRC_DIR)/types.h
locals.$(SUF):	$(EMH)/em_mes.h
locals.$(SUF):	$(EMH)/em_pseu.h
locals.$(SUF):	$(EMH)/em_spec.h
locals.$(SUF):	$(EMH)/em_mnem.h
init_glob.$(SUF):	$(SRC_DIR)/init_glob.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/init_glob.c
init_glob.$(SUF):	$(SRC_DIR)/map.h
init_glob.$(SUF):	$(SRC_DIR)/alloc.h
init_glob.$(SUF):	$(SRC_DIR)/global.h
init_glob.$(SUF):	$(SRC_DIR)/debug.h
init_glob.$(SUF):	$(SRC_DIR)/types.h
