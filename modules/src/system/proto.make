# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/modules/src/system
MOD_DIR = $(TARGET_HOME)/modules

LIBSYS	= libsystem.$(LIBSUF)
OBJ	= access.$(SUF) break.$(SUF) chmode.$(SUF) close.$(SUF) create.$(SUF) \
	  filesize.$(SUF) modtime.$(SUF) lock.$(SUF) open.$(SUF) read.$(SUF) \
	  remove.$(SUF) stop.$(SUF) system.$(SUF) time.$(SUF) unlock.$(SUF) \
	  write.$(SUF) seek.$(SUF) rename.$(SUF)
CSRC	= $(SRC_DIR)/access.c $(SRC_DIR)/break.c $(SRC_DIR)/chmode.c \
	  $(SRC_DIR)/close.c $(SRC_DIR)/create.c $(SRC_DIR)/filesize.c \
	  $(SRC_DIR)/modtime.c $(SRC_DIR)/lock.c $(SRC_DIR)/open.c \
	  $(SRC_DIR)/read.c $(SRC_DIR)/remove.c $(SRC_DIR)/stop.c \
	  $(SRC_DIR)/system.c $(SRC_DIR)/time.c $(SRC_DIR)/unlock.c \
	  $(SRC_DIR)/write.c $(SRC_DIR)/seek.c $(SRC_DIR)/rename.c
SRC	= $(SRC_DIR)/proto.make $(SRC_DIR)/system.h $(CSRC)

INCLUDES = -I$(SRC_DIR)
CFLAGS = $(COPTIONS) $(INCLUDES)

all:		$(LIBSYS)

$(LIBSYS):	$(OBJ)
		rm -f $(LIBSYS)
		$(AR) r $(LIBSYS) $(OBJ)
		$(RANLIB) $(LIBSYS)

install:	all
		cp $(LIBSYS) $(MOD_DIR)/lib/$(LIBSYS)
		$(RANLIB) $(MOD_DIR)/lib/$(LIBSYS)
		cp $(SRC_DIR)/system.3 $(MOD_DIR)/man/system.3
		cp $(SRC_DIR)/system.h $(MOD_DIR)/h/system.h
		if [ $(DO_MACHINE_INDEP) = y ] ; \
		then	mk_manpage $(SRC_DIR)/system.3 $(TARGET_HOME) ; \
		fi

cmp:		all
		-cmp $(LIBSYS) $(MOD_DIR)/lib/$(LIBSYS)
		-cmp $(SRC_DIR)/system.3 $(MOD_DIR)/man/system.3
		-cmp $(SRC_DIR)/system.h $(MOD_DIR)/h/system.h

clean:
		rm -f *.$(SUF) *.$(LIBSUF)

pr:
		@pr $(SRC)

opr:
		make pr | opr

lintlib:
		$(MK_LINT_LIB) system $(MOD_DIR)/lib $(INCLUDES) $(CSRC)

access.$(SUF):	$(SRC_DIR)/system.h $(SRC_DIR)/access.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/access.c

break.$(SUF):	$(SRC_DIR)/system.h $(SRC_DIR)/break.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/break.c

chmode.$(SUF):	$(SRC_DIR)/chmode.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/chmode.c

close.$(SUF):	$(SRC_DIR)/system.h $(SRC_DIR)/close.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/close.c

create.$(SUF):	$(SRC_DIR)/system.h $(SRC_DIR)/create.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/create.c

filesize.$(SUF): $(SRC_DIR)/filesize.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/filesize.c

lock.$(SUF):	$(SRC_DIR)/lock.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/lock.c

modtime.$(SUF):	$(SRC_DIR)/modtime.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/modtime.c

open.$(SUF):	$(SRC_DIR)/system.h $(SRC_DIR)/open.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/open.c

read.$(SUF):	$(SRC_DIR)/system.h $(SRC_DIR)/read.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/read.c

remove.$(SUF):	$(SRC_DIR)/remove.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/remove.c

rename.$(SUF):	$(SRC_DIR)/rename.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/rename.c

seek.$(SUF):	$(SRC_DIR)/seek.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/seek.c

stop.$(SUF):	$(SRC_DIR)/system.h $(SRC_DIR)/stop.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/stop.c

system.$(SUF):	$(SRC_DIR)/system.h $(SRC_DIR)/system.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/system.c

time.$(SUF):	$(SRC_DIR)/time.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/time.c

unlock.$(SUF):	$(SRC_DIR)/unlock.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/unlock.c

write.$(SUF):	$(SRC_DIR)/system.h $(SRC_DIR)/write.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/write.c
