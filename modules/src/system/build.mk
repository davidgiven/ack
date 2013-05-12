D := modules/src/system

define build-libsystem-impl
	$(call reset)
	$(call cfile, $D/access.c)
	$(call cfile, $D/break.c)
	$(call cfile, $D/chmode.c)
	$(call cfile, $D/close.c)
	$(call cfile, $D/create.c)
	$(call cfile, $D/filesize.c)
	$(call cfile, $D/open.c)
	$(call cfile, $D/read.c)
	$(call cfile, $D/remove.c)
	$(call cfile, $D/stop.c)
	$(call cfile, $D/system.c)
	$(call cfile, $D/time.c)
	$(call cfile, $D/write.c)
	$(call cfile, $D/seek.c)
	$(call cfile, $D/rename.c)
	$(call clibrary, $(LIBDIR)/libsystem.a)
	LIBSYSTEM := $o

	$(call reset)
	$(eval q := $D/system.h)
	$(call copyto, $(INCDIR)/system.h)
endef

$(eval $(build-libsystem-impl))

