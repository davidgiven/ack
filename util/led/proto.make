# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/led
LIBDIR= $(TARGET_HOME)/lib.bin
MODLIBDIR = $(TARGET_HOME)/modules/lib
PREFLAGS=  -I$(TARGET_HOME)/h -I$(TARGET_HOME)/config -DNDEBUG -DNASSERT
CFLAGS	= $(PREFLAGS) $(COPTIONS)
LINTFLAGS = $(PREFLAGS) $(LINTOPTIONS)
LDFLAGS	= $(LDOPTIONS)
LDLIBS = $(MODLIBDIR)/libstring.$(LIBSUF) $(MODLIBDIR)/libobject.$(LIBSUF)
LINTLIBS = \
	$(UTIL_HOME)/modules/lib/$(LINTPREF)string.$(LINTSUF) \
	$(UTIL_HOME)/modules/lib/$(LINTPREF)object.$(LINTSUF)

#	Some convenient macro definitions.

CFILES	= \
	$(SRC_DIR)/archive.c\
	$(SRC_DIR)/error.c\
	$(SRC_DIR)/extract.c\
	$(SRC_DIR)/finish.c\
	$(SRC_DIR)/main.c\
	$(SRC_DIR)/memory.c\
	$(SRC_DIR)/output.c\
	$(SRC_DIR)/read.c\
	$(SRC_DIR)/relocate.c\
	$(SRC_DIR)/save.c\
	$(SRC_DIR)/scan.c\
	$(SRC_DIR)/sym.c\
	$(SRC_DIR)/write.c

HFILES	= \
	$(SRC_DIR)/assert.h \
	$(SRC_DIR)/const.h \
	$(SRC_DIR)/debug.h \
	$(SRC_DIR)/defs.h \
	$(SRC_DIR)/memory.h \
	$(SRC_DIR)/orig.h \
	$(SRC_DIR)/scan.h
OFILES	= \
	archive.$(SUF) error.$(SUF) extract.$(SUF) finish.$(SUF) main.$(SUF) \
	memory.$(SUF) output.$(SUF) read.$(SUF) relocate.$(SUF) save.$(SUF) \
	scan.$(SUF) sym.$(SUF) write.$(SUF)

#	Things that can be made.

led:	$(OFILES)
	$(CC) $(LDFLAGS) $(OFILES) $(LDLIBS) -o led

install:led
	rm -f $(LIBDIR)/em_led $(TARGET_HOME)/man/led.6 $(TARGET_HOME)/man/ack.out.5
	cp led $(LIBDIR)/em_led
	cp led.6 $(TARGET_HOME)/man/led.6
	cp ack.out.5 $(TARGET_HOME)/man/ack.out.5

cmp:	led
	-cmp led $(LIBDIR)/em_led
	-cmp led.6 $(TARGET_HOME)/man/led.6
	-cmp ack.out.5 $(TARGET_HOME)/man/ack.out.5

lint:
	lint $(LINTFLAGS) $(CFILES) $(LINTLIBS)

pr:
	@pr $(PRFLAGS) $(SRC_DIR)/proto.make $(HFILES) $(CFILES)

opr:	
	make pr | opr

clean:
	rm -f Out *.$(SUF) led

depend:
	sed '/^#DEPENDENCIES/,$$d' Makefile >Makefile.new
	echo '#DEPENDENCIES' >>Makefile.new
	for i in $(CFILES) ; do \
		echo "`basename $$i .c`.$$(SUF):	$$i" >> Makefile.new ; \
		echo '	$$(CC) -c $$(CFLAGS)' $$i >> Makefile.new ; \
		$(UTIL_HOME)/lib.bin/cpp -d $(PREFLAGS) $$i | sed "s/^/`basename $$i .c`.$$(SUF):	/" >> Makefile.new ; \
	done
	mv Makefile Makefile.old
	mv Makefile.new Makefile

# do not remove the next line
#DEPENDENCIES
archive.$(SUF):	$(SRC_DIR)/archive.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/archive.c
archive.$(SUF):	$(SRC_DIR)/memory.h
archive.$(SUF):	$(SRC_DIR)/defs.h
archive.$(SUF):	$(SRC_DIR)/debug.h
archive.$(SUF):	$(SRC_DIR)/const.h
archive.$(SUF):	$(TARGET_HOME)/h/ranlib.h
archive.$(SUF):	$(TARGET_HOME)/h/out.h
archive.$(SUF):	$(TARGET_HOME)/h/arch.h
error.$(SUF):	$(SRC_DIR)/error.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/error.c
error.$(SUF):	$(SRC_DIR)/const.h
error.$(SUF):	$(TARGET_HOME)/h/out.h
extract.$(SUF):	$(SRC_DIR)/extract.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/extract.c
extract.$(SUF):	$(SRC_DIR)/scan.h
extract.$(SUF):	$(SRC_DIR)/orig.h
extract.$(SUF):	$(SRC_DIR)/memory.h
extract.$(SUF):	$(SRC_DIR)/defs.h
extract.$(SUF):	$(SRC_DIR)/debug.h
extract.$(SUF):	$(SRC_DIR)/const.h
extract.$(SUF):	$(TARGET_HOME)/h/out.h
finish.$(SUF):	$(SRC_DIR)/finish.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/finish.c
finish.$(SUF):	$(SRC_DIR)/scan.h
finish.$(SUF):	$(SRC_DIR)/orig.h
finish.$(SUF):	$(SRC_DIR)/memory.h
finish.$(SUF):	$(SRC_DIR)/defs.h
finish.$(SUF):	$(SRC_DIR)/const.h
finish.$(SUF):	$(TARGET_HOME)/h/out.h
main.$(SUF):	$(SRC_DIR)/main.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/main.c
main.$(SUF):	$(SRC_DIR)/orig.h
main.$(SUF):	$(SRC_DIR)/memory.h
main.$(SUF):	$(SRC_DIR)/defs.h
main.$(SUF):	$(SRC_DIR)/debug.h
main.$(SUF):	$(SRC_DIR)/const.h
main.$(SUF):	$(TARGET_HOME)/h/out.h
memory.$(SUF):	$(SRC_DIR)/memory.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/memory.c
memory.$(SUF):	$(TARGET_HOME)/config/local.h
memory.$(SUF):	$(SRC_DIR)/mach.c
memory.$(SUF):	$(SRC_DIR)/memory.h
memory.$(SUF):	$(SRC_DIR)/debug.h
memory.$(SUF):	$(SRC_DIR)/assert.h
memory.$(SUF):	$(SRC_DIR)/const.h
memory.$(SUF):	$(TARGET_HOME)/h/out.h
output.$(SUF):	$(SRC_DIR)/output.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/output.c
output.$(SUF):	$(SRC_DIR)/memory.h
output.$(SUF):	$(SRC_DIR)/const.h
output.$(SUF):	$(TARGET_HOME)/h/out.h
read.$(SUF):	$(SRC_DIR)/read.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/read.c
relocate.$(SUF):	$(SRC_DIR)/relocate.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/relocate.c
relocate.$(SUF):	$(SRC_DIR)/orig.h
relocate.$(SUF):	$(SRC_DIR)/defs.h
relocate.$(SUF):	$(SRC_DIR)/debug.h
relocate.$(SUF):	$(SRC_DIR)/const.h
relocate.$(SUF):	$(TARGET_HOME)/h/out.h
save.$(SUF):	$(SRC_DIR)/save.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/save.c
save.$(SUF):	$(SRC_DIR)/memory.h
save.$(SUF):	$(SRC_DIR)/assert.h
save.$(SUF):	$(SRC_DIR)/const.h
save.$(SUF):	$(TARGET_HOME)/h/out.h
save.$(SUF):	$(TARGET_HOME)/h/arch.h
scan.$(SUF):	$(SRC_DIR)/scan.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/scan.c
scan.$(SUF):	$(SRC_DIR)/debug.h
scan.$(SUF):	$(SRC_DIR)/scan.h
scan.$(SUF):	$(SRC_DIR)/memory.h
scan.$(SUF):	$(SRC_DIR)/assert.h
scan.$(SUF):	$(SRC_DIR)/const.h
scan.$(SUF):	$(TARGET_HOME)/h/ranlib.h
scan.$(SUF):	$(TARGET_HOME)/h/out.h
scan.$(SUF):	$(TARGET_HOME)/h/arch.h
sym.$(SUF):	$(SRC_DIR)/sym.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/sym.c
sym.$(SUF):	$(SRC_DIR)/debug.h
sym.$(SUF):	$(SRC_DIR)/memory.h
sym.$(SUF):	$(SRC_DIR)/const.h
sym.$(SUF):	$(TARGET_HOME)/h/out.h
write.$(SUF):	$(SRC_DIR)/write.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/write.c
write.$(SUF):	$(SRC_DIR)/memory.h
write.$(SUF):	$(SRC_DIR)/assert.h
write.$(SUF):	$(SRC_DIR)/const.h
write.$(SUF):	$(TARGET_HOME)/h/out.h
