d=..
h=$d/h
c=$d/util/data

FILES= \
$h/em_spec.h \
$h/em_pseu.h \
$h/em_mnem.h \
$c/em_flag.c \
$c/em_pseu.c \
$c/em_mnem.c

$(FILES):       em_table
		-mkdir $d/h
		-mkdir $d/util
		-mkdir $d/util/data
		new_table $h $c

install:	$(FILES)

distr:		$(FILES)
		touch new_table_done

opr:
		make pr ^ opr
pr:
		@pr Makefile em_table new_table pop_push traps

clean:
		-rm -f *.old

cmp :           # do nothing
