D := util/led

define build-led-impl
	$(call reset)
	$(call cfile, $D/archive.c)
	$(call cfile, $D/error.c)
	$(call cfile, $D/extract.c)
	$(call cfile, $D/finish.c)
	$(call cfile, $D/main.c)
	$(call cfile, $D/memory.c)
	$(call cfile, $D/output.c)
	$(call cfile, $D/read.c)
	$(call cfile, $D/relocate.c)
	$(call cfile, $D/save.c)
	$(call cfile, $D/scan.c)
	$(call cfile, $D/sym.c)
	$(call cfile, $D/write.c)

	$(call file, $(LIBSTRING))
	$(call file, $(LIBOBJECT))

	$(call cprogram, $(BINDIR)/em_led)
	$(call installto, $(PLATDEP)/em_led)

	$(call reset)
	$(eval q := $D/ack.out.5)
	$(call installto, $(INSDIR)/share/man/man5/ack.out.5)

	$(call reset)
	$(eval q := $D/led.6)
	$(call installto, $(INSDIR)/share/man/man6/led.6)
endef

$(eval $(build-led-impl))
