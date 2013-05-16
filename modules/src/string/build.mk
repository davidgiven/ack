D := modules/src/string

define build-libstring-impl
	$(call reset)
	$(call cfile, $D/bts2str.c)
	$(call cfile, $D/btscat.c)
	$(call cfile, $D/btscmp.c)
	$(call cfile, $D/btscpy.c)
	$(call cfile, $D/btszero.c)
	$(call cfile, $D/long2str.c)
	$(call cfile, $D/str2bts.c)
	$(call cfile, $D/str2long.c)
	$(call cfile, $D/strzero.c)
	$(call clibrary, $(LIBDIR)/libstring.a)
	LIBSTRING := $o
endef

$(eval $(build-libstring-impl))

