D := modules/src/print

define build-libprint-impl
	$(call reset)
	$(call cfile, $D/doprnt.c)
	$(call cfile, $D/fprint.c)
	$(call cfile, $D/print.c)
	$(call cfile, $D/sprint.c)
	$(call cfile, $D/format.c)

	$(eval $q: $(INCDIR)/system.h)

	$(call clibrary, $(LIBDIR)/libprint.a)
	LIBPRINT := $o

	$(call reset)
	$(eval q := $D/print.h)
	$(call copyto, $(INCDIR)/print.h)
endef

$(eval $(build-libprint-impl))

