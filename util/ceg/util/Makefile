EM = ../../..
CEGLIB = $(EM)/lib/ceg
UTIL = $(CEGLIB)/util
UtilList = make_as make_obj make_own

all:

install:
	-mkdir $(CEGLIB)
	-mkdir $(UTIL)
	cp $(UtilList) $(UTIL)
	cp install_ceg update_ceg $(EM)/bin

cmp:
	for i in $(UtilList) ; do cmp $$i $(UTIL)/$$i ; done

clean:

pr:
	@pr $(UtilList)

opr:
	make pr | opr
