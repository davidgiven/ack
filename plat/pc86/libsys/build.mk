D := plat/pc86/libsys

define build-pc86-libsys-impl
	$(call reset)
	$(call ackfile, $D/errno.s)
	$(call ackfile, $D/_hol0.s)
	$(call ackfile, $D/_sys_rawread.s)
	$(call ackfile, $D/_sys_rawwrite.s)
	$(call ackfile, $D/open.c)
	$(call ackfile, $D/creat.c)
	$(call ackfile, $D/close.c)
	$(call ackfile, $D/read.c)
	$(call ackfile, $D/write.c)
	$(call ackfile, $D/brk.c)
	$(call ackfile, $D/getpid.c)
	$(call ackfile, $D/kill.c)
	$(call ackfile, $D/isatty.c)
	$(call ackfile, $D/lseek.c)
	$(call ackfile, $D/time.c)
	$(call ackfile, $D/signal.c)
	$(call acklibrary, $(LIBDIR)/$(PLATFORM)/libsys.a)
	$(call installto, $(PLATIND)/$(PLATFORM)/libsys.a)
endef

$(eval $(build-pc86-libsys-impl))

