#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/modules/src/object
MOD_DIR = $(TARGET_HOME)/modules
INCLUDES = -I$(TARGET_HOME)/h -I$(MOD_DIR)/h -I$(TARGET_HOME)/config -I$(SRC_DIR)
CFLAGS = $(INCLUDES) $(COPTIONS)

CFILES =	$(SRC_DIR)/rd_arhdr.c $(SRC_DIR)/wr_arhdr.c \
		$(SRC_DIR)/rd_ranlib.c $(SRC_DIR)/wr_ranlib.c \
		$(SRC_DIR)/rd_bytes.c $(SRC_DIR)/wr_bytes.c \
		$(SRC_DIR)/rd.c $(SRC_DIR)/wr.c \
		$(SRC_DIR)/wr_putc.c \
		$(SRC_DIR)/rd_int2.c $(SRC_DIR)/wr_int2.c \
		$(SRC_DIR)/rd_unsig2.c \
		$(SRC_DIR)/rd_long.c $(SRC_DIR)/wr_long.c
# do not change the order in OFILES
OFILES =	rd.$(SUF) rd_arhdr.$(SUF) rd_int2.$(SUF) rd_long.$(SUF) \
		rd_ranlib.$(SUF) rd_unsig2.$(SUF) rd_bytes.$(SUF) \
		wr_arhdr.$(SUF) wr_int2.$(SUF) wr_long.$(SUF) wr_putc.$(SUF) \
		wr.$(SUF) wr_ranlib.$(SUF) wr_bytes.$(SUF)

all:		libobject.$(LIBSUF)

install:	all
		-mkdir $(MOD_DIR)/lib
		-mkdir $(MOD_DIR)/h
		cp libobject.$(LIBSUF) $(MOD_DIR)/lib/libobject.$(LIBSUF)
		$(RANLIB) $(MOD_DIR)/lib/libobject.$(LIBSUF)
		cp $(SRC_DIR)/object.h $(MOD_DIR)/h/object.h
		if [ $(DO_MACHINE_INDEP) = y ] ; \
		then	mk_manpage $(SRC_DIR)/object.3 $(TARGET_HOME) ; \
		fi

compare:	all
		-cmp libobject.$(LIBSUF) $(MOD_DIR)/lib/libobject.$(LIBSUF)
		-cmp $(SRC_DIR)/object.h $(MOD_DIR)/h/object.h

pr:
		@pr $(SRC_DIR)/proto.make $(SRC_DIR)/object.h $(CFILES)

opr:
		make pr | opr

clean:
		rm -f *.$(SUF) *.$(LIBSUF) nohup.out Out

libobject.$(LIBSUF):	$(OFILES)
		rm -f libobject.$(LIBSUF)
		$(AR) r libobject.$(LIBSUF) $(OFILES)
		$(RANLIB) libobject.$(LIBSUF)

lintlib:
		$(MK_LINT_LIB) object $(MOD_DIR)/lib $(INCLUDES) $(CFILES)
		
rd_arhdr.$(SUF):	$(SRC_DIR)/object.h $(SRC_DIR)/rd_arhdr.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/rd_arhdr.c

wr_arhdr.$(SUF):	$(SRC_DIR)/object.h $(SRC_DIR)/wr_arhdr.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/wr_arhdr.c

rd_ranlib.$(SUF):	$(SRC_DIR)/object.h $(SRC_DIR)/rd_ranlib.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/rd_ranlib.c

wr_ranlib.$(SUF):	$(SRC_DIR)/object.h $(SRC_DIR)/wr_ranlib.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/wr_ranlib.c

rd.$(SUF):	$(SRC_DIR)/object.h $(SRC_DIR)/rd.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/rd.c

wr.$(SUF):	$(SRC_DIR)/object.h $(SRC_DIR)/wr.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/wr.c

wr_putc.$(SUF):	$(SRC_DIR)/object.h $(SRC_DIR)/wr_putc.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/wr_putc.c

rd_int2.$(SUF):	$(SRC_DIR)/object.h $(SRC_DIR)/rd_int2.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/rd_int2.c

wr_int2.$(SUF):	$(SRC_DIR)/object.h $(SRC_DIR)/wr_int2.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/wr_int2.c

rd_unsig2.$(SUF):	$(SRC_DIR)/object.h $(SRC_DIR)/rd_unsig2.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/rd_unsig2.c

rd_long.$(SUF):	$(SRC_DIR)/object.h $(SRC_DIR)/rd_long.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/rd_long.c

wr_long.$(SUF):	$(SRC_DIR)/object.h $(SRC_DIR)/wr_long.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/wr_long.c

rd_bytes.$(SUF):	$(SRC_DIR)/rd_bytes.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/rd_bytes.c

wr_bytes.$(SUF):	$(SRC_DIR)/wr_bytes.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/wr_bytes.c
