# $Header$

#PARAMS		do not remove this line

SRC_DIR = $(SRC_HOME)/util/ego/em_ego
EMH=$(TARGET_HOME)/h
EMCONFIG=$(TARGET_HOME)/config
EMLIB=$(TARGET_HOME)/lib.bin
MODLIB=$(TARGET_HOME)/modules/lib
MODH=$(TARGET_HOME)/modules/h
MODS=$(MODLIB)/libprint.$(LIBSUF) $(MODLIB)/libstring.$(LIBSUF) $(MODLIB)/libsystem.$(LIBSUF)

LDFLAGS=$(LDOPTIONS)
CPPFLAGS=-DVERBOSE -DNOTCOMPACT -I$(EMH) -I$(EMCONFIG) -I$(MODH)
CFLAGS=$(CPPFLAGS) $(COPTIONS)
LINTFLAGS=$(CPPFLAGS) $(LINTOPTIONS)

all:	em_ego

em_ego:	em_ego.$(SUF)
	$(CC) $(LDFLAGS) -o em_ego em_ego.$(SUF) $(MODS)

em_ego.$(SUF):	$(SRC_DIR)/em_ego.c $(EMCONFIG)/em_path.h $(MODH)/system.h
	$(CC) -c $(CFLAGS) $(SRC_DIR)/em_ego.c

install:	all
	rm -f $(EMLIB)/em_ego
	cp em_ego $(EMLIB)/em_ego

cmp:	all
	-cmp em_ego $(EMLIB)/em_ego

pr:
	@pr $(SRC_DIR)/proto.make $(SRC_DIR)/em_ego.c

opr:
	make pr | opr

lint:
	$(LINT) $(LINTFLAGS) $(SRC_DIR)/em_ego.c

clean:
	rm -f *.$(SUF) em_ego
