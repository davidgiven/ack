EM = ../../..
CEGLIB = $(EM)/lib/ceg
UTIL = $(CEGLIB)/util
UtilList = create_ofiles make_as make_obj make_own update

all:

install:
	-mkdir $(CEGLIB)
	-mkdir $(UTIL)
	cp $(UtilList) $(UTIL)
	cp install_ceg $(EM)/bin

cmp:
	for i in $(UtilList) ; do cmp $$i $(UTIL)/$$i ; done

clean:

pr:
	@pr $(UtilList)

opr:
	make pr | opr
