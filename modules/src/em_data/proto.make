# $Id$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/data
OBJ = em_mnem.$(SUF) em_pseu.$(SUF) em_flag.$(SUF) em_ptyp.$(SUF)

DATA_PATH = em_data.$(LIBSUF)

INCLUDES = -I$(TARGET_HOME)/h
CFLAGS = $(COPTIONS) $(INCLUDES)

$(DATA_PATH):   $(OBJ)
		$(AR) rv $(DATA_PATH) $(OBJ)
		$(RANLIB) $(DATA_PATH)

install:	$(DATA_PATH)
		cp $(DATA_PATH) $(TARGET_HOME)/lib.bin/$(DATA_PATH)
		$(RANLIB) $(TARGET_HOME)/lib.bin/$(DATA_PATH)


cmp:		$(DATA_PATH)
		-cmp $(DATA_PATH) $(TARGET_HOME)/lib.bin/$(DATA_PATH)

clean:
		rm -f $(OBJ) $(DATA_PATH) *.old

opr:
		make pr ^ opr

pr:
		@pr $(SRC_DIR)/proto.make $(SRC_DIR)/em_mnem.c $(SRC_DIR)/em_pseu.c $(SRC_DIR)/em_flag.c $(SRC_DIR)/em_ptyp.c

em_flag.$(SUF):	$(SRC_DIR)/em_flag.c $(TARGET_HOME)/h/em_flag.h
		$(CC) -c $(CFLAGS) $(SRC_DIR)/em_flag.c

em_mnem.$(SUF):	$(SRC_DIR)/em_mnem.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/em_mnem.c

em_pseu.$(SUF):	$(SRC_DIR)/em_pseu.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/em_pseu.c

em_ptyp.$(SUF):	$(SRC_DIR)/em_ptyp.c $(TARGET_HOME)/h/em_spec.h $(TARGET_HOME)/h/em_ptyp.h
		$(CC) -c $(CFLAGS) $(SRC_DIR)/em_ptyp.c

lintlib:
		$(MK_LINT_LIB) em_data $(TARGET_HOME)/lib.bin $(INCLUDES) \
			$(SRC_DIR)/em_flag.c $(SRC_DIR)/em_mnem.c \
			$(SRC_DIR)/em_pseu.c $(SRC_DIR)/em_ptyp.c
