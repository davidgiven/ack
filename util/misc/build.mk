D := util/misc

define build-misc-impl
	$(call reset)
	$(call cfile, $D/esize.c)
	$(call cprogram, $(BINDIR)/esize)
	$(call installto, $(INSDIR)/bin/esize)

	$(call reset)
	$(eval q := $D/esize.1)
	$(call installto, $(INSDIR)/share/man/man1/esize.1)

	$(call reset)
	$(eval objdir := encode)
	$(call cfile, $D/convert.c)
	$(eval $q: $(INCDIR)/em_comp.h $(INCDIR)/em_codeEK.h)
	$(call file, $(LIBREAD_EMEV))
	$(call file, $(LIBEMK))
	$(call file, $(LIBEM_DATA))
	$(call file, $(LIBALLOC))
	$(call file, $(LIBPRINT))
	$(call file, $(LIBSTRING))
	$(call file, $(LIBSYSTEM))
	$(call cprogram, $(BINDIR)/em_encode)
	$(call installto, $(PLATDEP)/em_encode)
	$(eval EM_ENCODE := $o)
	$(eval ACK_CORE_TOOLS += $o)

	$(call reset)
	$(eval objdir := decode)
	$(call cfile, $D/convert.c)
	$(eval $q: $(INCDIR)/em_comp.h $(INCDIR)/em_codeEK.h)
	$(call file, $(LIBREAD_EMKV))
	$(call file, $(LIBEME))
	$(call file, $(LIBEM_DATA))
	$(call file, $(LIBALLOC))
	$(call file, $(LIBPRINT))
	$(call file, $(LIBSTRING))
	$(call file, $(LIBSYSTEM))
	$(call cprogram, $(BINDIR)/em_decode)
	$(call installto, $(PLATDEP)/em_decode)
	$(eval EM_DECODE := $o)

	$(call reset)
	$(eval q := $D/em_decode.6)
	$(call installto, $(INSDIR)/share/man/man6/em_decode.6)
endef

$(eval $(build-misc-impl))
