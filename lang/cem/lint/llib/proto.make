# (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
# See the copyright notice in the ACK home directory, in the file "Copyright".
#
# $Header$

#PARAMS		do not remove this line!

SRC_DIR =	$(SRC_HOME)/lang/cem/lint/llib

LINT =		$(TARGET_HOME)/bin/lint

CURSESDIR =	/usr/src/lib/libcurses#		# for a local -lcurses
MATHDIR =	/usr/src/lib/libm#		# for a local -lm
TERMLIBDIR =	/usr/src/lib/libtermlib#	# for a local -ltermlib

CLIBDIR =	$(SRC_HOME)/lang/cem/libcc#	# to create c.llb
ANSILIBDIR =	$(SRC_HOME)/lang/cem/libcc.ansi## to create c.llb
INCLUDE =	$(SRC_HOME)/include/_tail_cc
AINCLUDE =	$(SRC_HOME)/lang/cem/libcc.ansi/headers
CLIBFLAGS =	-I$(INCLUDE)			# to compile $(CLIBDIR)
ANSILIBFLAGS =	-I$(AINCLUDE)			# to compile $(ANSILIBDIR)

all:	c.llb ansi_c.llb

install:	all
	-mkdir $(TARGET_HOME)/lib.bin/lint
	-mkdir $(TARGET_HOME)/lib.bin/lint/llib
	cp $(SRC_DIR)/*.llb $(TARGET_HOME)/lib.bin/lint/llib
	cp *.llb $(TARGET_HOME)/lib.bin/lint/llib
	chmod +w $(TARGET_HOME)/lib.bin/lint/llib/*

cmp:
	echo "nothing compared"

pr:
	echo "nothing printed"

opr:
	echo "nothing printed"

clean:
	rm -f *.llb Out

c.llb:	$(SRC_DIR)/unix7.c
	cp $(SRC_DIR)/c.llb c.llb
	chmod +w c.llb
	$(LINT) -Lc.llb $(SRC_DIR)/unix7.c

ansi_c.llb:	$(SRC_DIR)/unix7.c
	cp $(SRC_DIR)/ansi_c.llb ansi_c.llb
	chmod +w ansi_c.llb
	$(LINT) -ansi -Lansi_c.llb $(SRC_DIR)/unix7.c

curses.llb:
	$(LINT) -Lcurses.llb $(CURSESDIR)/*.c

m.llb:
	$(LINT) -Lm.llb $(MATHDIR)/*.c

termlib.llb:
	$(LINT) -Ltermlib.llb $(TERMLIBDIR)/*.c

cnew.llb:
	cp /dev/null cnew.llb
	$(LINT) -Lcnew.llb $(CLIBFLAGS) $(CLIBDIR)/*/*.c
	$(LINT) -Lcnew.llb $(SRC_DIR)/unix7.c

ansi_cnew.llb:
	cp /dev/null ansi_cnew.llb
	$(LINT) -ansi -Lansi_cnew.llb $(ANSILIBFLAGS) $(ANSILIBDIR)/*/*.c
	$(LINT) -Lansi_cnew.llb $(SRC_DIR)/unix7.c
