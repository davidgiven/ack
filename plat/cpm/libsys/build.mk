D := plat/cpm/libsys

define build-cpm-libsys-impl
	$(call reset)
	$(call ackfile, $D/_bdos.s)
	$(call ackfile, $D/_hol0.s)
	$(call ackfile, $D/_inn2.s)
	$(call ackfile, $D/_trap.s)
	$(call ackfile, $D/brk.c)
	$(call ackfile, $D/close.c)
	$(call ackfile, $D/creat.c)
	$(call ackfile, $D/errno.s)
	$(call ackfile, $D/getpid.c)
	$(call ackfile, $D/isatty.c)
	$(call ackfile, $D/kill.c)
	$(call ackfile, $D/lseek.c)
	$(call ackfile, $D/open.c)
	$(call ackfile, $D/read.c)
	$(call ackfile, $D/signal.c)
	$(call ackfile, $D/time.c)
	$(call ackfile, $D/write.c)

	$(call acklibrary, $(LIBDIR)/$(PLATFORM)/libsys.a)
	$(call installto, $(PLATIND)/$(PLATFORM)/libsys.a)
endef

$(eval $(build-cpm-libsys-impl))

