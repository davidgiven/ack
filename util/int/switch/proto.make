# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/int/switch
IP_SPEC =	$(SRC_HOME)/etc/ip_spec.t
UCFLAGS =	-I$(UTIL_HOME) -I$(SRC_HOME) $(COPTIONS)
ULDFLAGS =	$(ULDOPTIONS)
LINTFLAGS =	-I$(UTIL_HOME) -I$(SRC_HOME) $(LINTOPTIONS)

SRC =		$(SRC_DIR)/mkiswitch.c	$(SRC_DIR)/mkswitch.c
OBJ =		mkiswitch.$(USUF)	mkswitch.$(USUF)

mkiswitch:	mkiswitch.$(USUF)
	$(UCC) $(ULDFLAGS) -o mkiswitch mkiswitch.$(USUF) $(UTIL_HOME)/lib.bin/em_data.$(ULIBSUF)

mkiswitch.$(USUF):	$(SRC_DIR)/mkiswitch.c
	$(UCC) $(UCFLAGS) -c $(SRC_DIR)/mkiswitch.c

mkswitch:	mkswitch.$(USUF)
	$(UCC) $(ULDFLAGS) -o mkswitch mkswitch.$(USUF)

mkswitch.$(USUF):	$(SRC_DIR)/mkswitch.c
	$(UCC) $(UCFLAGS) -c $(SRC_DIR)/mkswitch.c

DoCases:	mkiswitch $(IP_SPEC)
	mkiswitch Do $(IP_SPEC) DoCases
	wc DoCases

PrCases:	mkswitch $(IP_SPEC)
	mkswitch Pr $(IP_SPEC) PrCases
	wc PrCases

lint:
	$(LINT) $(LINTOPTIONS) $(SRC_DIR)/mkiswitch.c $(UTIL_HOME)/lib.bin/em_data.$(LINTSUF)
	$(LINT) $(LINTOPTIONS) $(SRC_DIR)/mkswitch.c

clean:						# NOT the cases files !
	rm -f a.out core $(OBJ) mkswitch mkiswitch

bare:		clean
	rm -f DoCases PrCases

all:	mkswitch

install:
	echo "Nothing to install"

cmp:
	echo "Nothing to compare"

pr:
	@pr $(SRC_DIR)/READ_ME $(SRC_DIR)/proto.make $(SRC)

opr:
	make pr | opr
