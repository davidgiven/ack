# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/modules/src/alloc
MOD_DIR = $(TARGET_HOME)/modules
INCLUDES = -I$(SRC_DIR) -I$(MOD_DIR)/h
CFLAGS = $(INCLUDES) $(COPTIONS)

CSRC =		$(SRC_DIR)/Malloc.c\
		$(SRC_DIR)/Salloc.c\
		$(SRC_DIR)/Srealloc.c\
		$(SRC_DIR)/Realloc.c\
		$(SRC_DIR)/botch.c\
		$(SRC_DIR)/clear.c\
		$(SRC_DIR)/st_alloc.c\
		$(SRC_DIR)/std_alloc.c \
		$(SRC_DIR)/No_Mem.c
SOURCES =	$(SRC_DIR)/alloc.h\
		$(CSRC)

OBJECTS =	botch.$(SUF) clear.$(SUF) st_alloc.$(SUF) Malloc.$(SUF) \
		Salloc.$(SUF) \
		Srealloc.$(SUF) Realloc.$(SUF) std_alloc.$(SUF) No_Mem.$(SUF)


LIBALLOC =	liballoc.$(LIBSUF)

all:		$(LIBALLOC)

$(LIBALLOC):	$(OBJECTS)
		rm -f $(LIBALLOC)
		$(AR) r $(LIBALLOC) $(OBJECTS)
		$(RANLIB) $(LIBALLOC)

install:	all lintlib
		cp $(LIBALLOC) $(MOD_DIR)/lib/$(LIBALLOC)
		$(RANLIB) $(MOD_DIR)/lib/$(LIBALLOC)
		cp $(SRC_DIR)/alloc.3 $(MOD_DIR)/man/alloc.3
		cp $(SRC_DIR)/alloc.h $(MOD_DIR)/h/alloc.h

cmp:		all
		-cmp $(LIBALLOC) $(MOD_DIR)/lib/$(LIBALLOC)
		-cmp $(SRC_DIR)/alloc.3 $(MOD_DIR)/man/alloc.3
		-cmp $(SRC_DIR)/alloc.h $(MOD_DIR)/h/alloc.h

pr:
		@pr $(SRC_DIR)/proto.make $(SOURCES)

opr:
		make pr | opr

clean:
		rm -f *.$(SUF) *.$(LIBSUF)

lintlib:
		$(MK_LINT_LIB) alloc $(MOD_DIR)/lib $(INCLUDES) $(CSRC)

st_alloc.$(SUF):	$(SRC_DIR)/alloc.h $(SRC_DIR)/st_alloc.c
			$(CC) -c $(CFLAGS) $(SRC_DIR)/st_alloc.c

std_alloc.$(SUF):	$(SRC_DIR)/alloc.h $(SRC_DIR)/std_alloc.c
			$(CC) -c $(CFLAGS) $(SRC_DIR)/std_alloc.c

Malloc.$(SUF):		$(SRC_DIR)/alloc.h $(SRC_DIR)/Malloc.c
			$(CC) -c $(CFLAGS) $(SRC_DIR)/Malloc.c

No_Mem.$(SUF):		$(SRC_DIR)/No_Mem.c
			$(CC) -c $(CFLAGS) $(SRC_DIR)/No_Mem.c

Realloc.$(SUF):		$(SRC_DIR)/Realloc.c
			$(CC) -c $(CFLAGS) $(SRC_DIR)/Realloc.c

Salloc.$(SUF):		$(SRC_DIR)/Salloc.c
			$(CC) -c $(CFLAGS) $(SRC_DIR)/Salloc.c

Srealloc.$(SUF):	$(SRC_DIR)/Srealloc.c
			$(CC) -c $(CFLAGS) $(SRC_DIR)/Srealloc.c

botch.$(SUF):		$(SRC_DIR)/botch.c
			$(CC) -c $(CFLAGS) $(SRC_DIR)/botch.c

clear.$(SUF):		$(SRC_DIR)/clear.c
			$(CC) -c $(CFLAGS) $(SRC_DIR)/clear.c
