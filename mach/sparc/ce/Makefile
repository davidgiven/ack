SED = sed
M4 = m4

all:	EM_table cache.c

EM_table:
	if [ -x /usr/ccs/lib/cpp ] ; then /usr/ccs/lib/cpp -P < EM_table.x | $(M4) | $(SED) -f cegpp > $@ ; else /lib/cpp -P < EM_table.x | $(M4) | $(SED) -f cegpp > $@ ; fi
	
cache.c:
	$(SED) -f cegpp cache.c.x > $@

distr:
	rm -f EM_table cache.c
	make all
