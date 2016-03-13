D := util/arch

define build-aal-impl
	$(call reset)
	$(eval cflags += -DAAL)

	$(call cfile, $D/archiver.c)

	$(call rawfile, $(LIBOBJECT))
	$(call rawfile, $(LIBPRINT))
	$(call rawfile, $(LIBSTRING))
	$(call rawfile, $(LIBSYSTEM))

	$(call cprogram, $(BINDIR)/aal)
	$(call installto, $(INSDIR)/bin/aal)
	$(eval AAL := $o)
	$(eval ACK_CORE_TOOLS += $o)

	$(call reset)
	$(eval q := $D/aal.1)
	$(call installto, $(INSDIR)/share/man/man1/aal.1)
endef

$(eval $(build-aal-impl))
