# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/lang/cem/libcc.ansi

all:

install:
		
	-mkdir $(TARGET_HOME)/include
	-mkdir $(TARGET_HOME)/include/tail_ac
	( cd $(SRC_DIR)/headers; tar cf - `cat LIST` ) | ( cd $(TARGET_HOME)/include/tail_ac ; tar xf - )
