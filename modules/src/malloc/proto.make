# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/modules/src/malloc
MOD_DIR = $(TARGET_HOME)/modules
INCLUDES = -I$(MOD_DIR)/h
CFLAGS = $(INCLUDES) $(COPTIONS)

MALLOCSRC =	$(SRC_DIR)/READ_ME $(SRC_DIR)/size_type.h \
		gensize_type.h $(SRC_DIR)/param.h $(SRC_DIR)/impl.h \
		$(SRC_DIR)/check.h $(SRC_DIR)/log.h $(SRC_DIR)/phys.h \
		$(SRC_DIR)/mal.c $(SRC_DIR)/log.c $(SRC_DIR)/phys.c \
		$(SRC_DIR)/check.c

all:		malloc.$(SUF)

install:	all
		cp malloc.$(SUF) $(MOD_DIR)/lib/malloc.$(SUF)

cmp:		all
		-cmp malloc.$(SUF) $(MOD_DIR)/lib/malloc.$(SUF)

malloc1.c:	$(MALLOCSRC) $(SRC_DIR)/add_file
		rm -f malloc1.c
		for i in $(MALLOCSRC) ; do $(SRC_DIR)/add_file $$i >> malloc1.c ; done

malloc.c:	malloc1.c
		cclash -l7 -c malloc1.c > clashes
		cid -Fclashes < malloc1.c > malloc.c

pr:
		@pr $(SRC_DIR)/proto.make $(SRC_DIR)/add_file $(MALLOCSRC)

opr:
		make pr | opr

clean:
		rm -f *.$(SUF) clashes malloc1.c gensize_type.h getsize malloc.c

gensize_type.h:	getsize
		getsize > gensize_type.h

getsize:	$(SRC_DIR)/getsize.c
		cc -o getsize $(SRC_DIR)/getsize.c
# use cc, not $(CC) as this must produce runnable binary.

malloc.$(SUF):	malloc.c
		$(CC) -c $(CFLAGS) malloc.c
# to avoid options that the SUN make adds.
