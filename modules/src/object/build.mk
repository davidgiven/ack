D := modules/src/object

define build-libobject-impl
	$(call reset)
	$(call cfile, $D/rd.c)
	$(call cfile, $D/rd_arhdr.c)
	$(call cfile, $D/rd_bytes.c)
	$(call cfile, $D/rd_int2.c)
	$(call cfile, $D/rd_long.c)
	$(call cfile, $D/rd_ranlib.c)
	$(call cfile, $D/rd_unsig2.c)
	$(call cfile, $D/wr.c)
	$(call cfile, $D/wr_arhdr.c)
	$(call cfile, $D/wr_bytes.c)
	$(call cfile, $D/wr_int2.c)
	$(call cfile, $D/wr_long.c)
	$(call cfile, $D/wr_putc.c)
	$(call cfile, $D/wr_ranlib.c)

	$(eval $q: $(INCDIR)/local.h)

	$(call clibrary, $(LIBDIR)/libobject.a)
	LIBOBJECT := $o
endef

$(eval $(build-libobject-impl))

