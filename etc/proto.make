# $Id$

#PARAMS		do not remove this line!

h=$(TARGET_HOME)/h
c=$(SRC_HOME)/util/data

FILES= \
$h/em_spec.h \
$h/em_pseu.h \
$h/em_mnem.h \
$c/em_flag.c \
$c/em_pseu.c \
$c/em_mnem.c

$(FILES):       $(SRC_DIR)/etc/em_table
		cd $(SRC_DIR)/etc ; new_table $h $c

install:	$(FILES)

distr:		$(FILES)

opr:
		make pr ^ opr
pr:
		@cd $(SRC_DIR) ; pr proto.make em_table new_table pop_push traps

clean:
		-rm -f *.old

cmp :           # do nothing
