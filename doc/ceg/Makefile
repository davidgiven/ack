PIC=pic
TBL=tbl
TARGET=-Tlp

../ceg.doc:	ceg.tr ceg.ref
	$(PIC) $(TARGET) ceg.tr | refer -e -p ceg.ref | $(TBL) > $@
