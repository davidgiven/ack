PIC=pic
TBL=tbl
REFER=refer

../ceg.doc:	ceg.tr ceg.ref
	$(PIC) ceg.tr | $(REFER) -e -p ceg.ref | $(TBL) > $@
