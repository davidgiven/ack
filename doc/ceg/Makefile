PIC=pic
TBL=tbl
TARGET=-Tlp
REFER=refer

../ceg.doc:	ceg.tr ceg.ref
	$(PIC) $(TARGET) ceg.tr | $(REFER) -e -p ceg.ref | $(TBL) > $@
