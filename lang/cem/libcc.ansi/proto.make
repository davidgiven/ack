# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/lang/cem/libcc.ansi

all:

install:
	if [ $(DO_MACHINE_INDEP) = y ] ; \
	then cp_dir $(SRC_DIR)/headers $(TARGET_HOME)/include/tail_ac ; \
	fi

clean:

pr:

opr:

cmp:
