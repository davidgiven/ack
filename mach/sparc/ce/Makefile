SED = sed
M4 = m4
CPP = /lib/cpp

all:	EM_table cache.c

distr:	all

EM_table:	EM_table.x cegpp
	$(CPP) < EM_table.x -P | $(M4) | $(SED) -f cegpp > $@
	
cache.c:	cache.c.x cegpp
	$(SED) -f cegpp cache.c.x > $@
