# $Header$
LIB=/usr/lib
MIDDLE=tmac
# the script replaces references to pathnames and
# deletes all comment
RUNSCRIPT=sed -e s-LIBDIR-$(LIB)-g -e '/Header/!s/[ 	]*\\".*//' -e '/^\.$$/d' -e 's/^\.[ 	][ 	]*/./'
FILES=tmac.kun tmac.q?

TBL=tbl
REFER=refer
SUF=pr
NROFF=nroff

head:		$(FILES)

pubmac.$(SUF):	pubmac
		sed 's/^\.UW/.BW/' < pubmac | $(TBL) | \
			$(REFER) | $(NROFF) -kun > pubmac.$(SUF)

install:	install_files

install_files:
		for i in $(FILES) ;\
		do\
			rm -f $(LIB)/$(MIDDLE)/$$i ;\
			$(RUNSCRIPT) $$i >$(LIB)/$(MIDDLE)/$$i ;\
			chmod -w $(LIB)/$(MIDDLE)/$$i ;\
		done

clean:

pr:

opr:

cmp:
		echo "no cmp"
