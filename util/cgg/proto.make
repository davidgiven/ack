# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/util/cgg
INCLUDES=-I$(TARGET_HOME)/h -I$(TARGET_HOME)/config -I$(SRC_DIR) -I.
CFLAGS=$(INCLUDES) $(COPTIONS)
LDFLAGS=$(LDOPTIONS)
LINTFLAGS=$(INCLUDES) $(LINTOPTIONS)
LIBS=$(TARGET_HOME)/lib.bin/em_data.$(LIBSUF)

cgg:	bootgram.$(SUF) main.$(SUF) bootlex.$(SUF)
	$(CC) $(LDFLAGS) bootgram.$(SUF) main.$(SUF) bootlex.$(SUF) $(LIBS) -o cgg

bootgram.c:	$(SRC_DIR)/bootgram.y
	@echo expect 1 shift/reduce conflict
	yacc -d $(SRC_DIR)/bootgram.y
	mv y.tab.c bootgram.c

install:	cgg
	cp cgg $(TARGET_HOME)/lib.bin/cgg

cmp:		cgg
	-cmp cgg $(TARGET_HOME)/lib.bin/cgg

bootlex.c:	$(SRC_DIR)/bootlex.l
	flex -st $(SRC_DIR)/bootlex.l > bootlex.c

lint:	bootgram.c $(SRC_DIR)/main.c bootlex.c
	$(LINT) $(LINTFLAGS) bootgram.c $(SRC_DIR)/main.c bootlex.c $(UTIL_HOME)/lib.bin/$(LINTPREF)em_data.$(LINTSUF)
clean:
	rm -f *.$(SUF) bootgram.c bootlex.c cgg y.tab.h

bootgram.$(SUF):	bootgram.c
		$(CC) -c $(CFLAGS) bootgram.c
bootgram.$(SUF):	$(SRC_DIR)/booth.h
bootgram.$(SUF):	$(TARGET_HOME)/h/cg_pattern.h
bootlex.$(SUF):	bootlex.c
		$(CC) -c $(CFLAGS) bootlex.c
bootlex.$(SUF):	$(SRC_DIR)/booth.h
bootlex.$(SUF):	$(TARGET_HOME)/h/cg_pattern.h
main.$(SUF):	$(SRC_DIR)/main.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/main.c
main.$(SUF):	$(SRC_DIR)/booth.h
main.$(SUF):	$(TARGET_HOME)//h/cg_pattern.h
