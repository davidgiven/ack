# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/mach/z80/int

SRC =	$(SRC_DIR)/em.s $(SRC_DIR)/atof.s $(SRC_DIR)/fpp.s $(SRC_DIR)/mli4.s \
	$(SRC_DIR)/dvu4.s $(SRC_DIR)/dvi4.s $(SRC_DIR)/eb.s
OBJ =	em.o atof.o fpp.o mli4.o dvu4.o dvi4.o eb.o

DLSRC =	$(SRC_DIR)/dl.c $(SRC_DIR)/dlbin.c
LIBOBJ = $(UTIL_HOME)/modules/lib/libobject.$(ULIBSUF)

ULDFLAGS = $(ULDOPTIONS)
UCFLAGS = -I$(UTIL_HOME)/h $(UCOPTIONS)

all:	int.hex dl dlbin cv

install:	all
	@echo "nothing is installed"

cmp:	all
	@echo "nothing is compared"

clean:
	rm -f *.o a.out dl dlbin *.hex cv
	rm -f *.$(USUF)

pr:
	@pr $(SRC_DIR)/proto.make $(SRC) $(DLSRC) $(SRC_DIR)/cv.c

opr:
	make pr | opr

a.out:	$(OBJ)
	$(UTIL_HOME)/lib.bin/em_led -b0:0x100 -a0:2 -a1:2 -a2:2 -a3:2 $(OBJ)

int.hex:	a.out dlbin
	./dlbin a.out > int.hex
	./dosort int.hex

dl:	dl.$(USUF)
	$(UCC) $(ULDFLAGS) -o dl dl.$(USUF)

dl.$(USUF):	$(SRC_DIR)/dl.c
	$(UCC) $(UCFLAGS) -c $(SRC_DIR)/dl.c

dlbin:	dlbin.$(USUF)
	$(UCC) $(ULDFLAGS) -o dlbin dlbin.$(USUF) $(LIBOBJ)

dlbin.$(USUF):	$(SRC_DIR)/dlbin.c
	$(UCC) $(UCFLAGS) -c $(SRC_DIR)/dlbin.c

cv:	cv.$(USUF)
	$(CC) $(ULDFLAGS) -o cv cv.$(USUF)

cv.$(USUF):	$(SRC_DIR)/cv.c
	$(UCC) $(UCFLAGS) -c $(SRC_DIR)/cv.c

atof.o:	$(SRC_DIR)/atof.s
	z80 -I$(TARGET_HOME)/h -c $(SRC_DIR)/atof.s
fpp.o:	$(SRC_DIR)/fpp.s
	z80 -I$(TARGET_HOME)/h -c $(SRC_DIR)/fpp.s
mli4.o:	$(SRC_DIR)/mli4.s
	z80 -I$(TARGET_HOME)/h -c $(SRC_DIR)/mli4.s
dvu4.o:	$(SRC_DIR)/dvu4.s
	z80 -I$(TARGET_HOME)/h -c $(SRC_DIR)/dvu4.s
dvi4.o:	$(SRC_DIR)/dvi4.s
	z80 -I$(TARGET_HOME)/h -c $(SRC_DIR)/dvi4.s
eb.o:	$(SRC_DIR)/eb.s
	z80 -I$(TARGET_HOME)/h -c $(SRC_DIR)/eb.s
