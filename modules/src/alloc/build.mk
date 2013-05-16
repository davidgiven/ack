D := modules/src/alloc

define build-liballoc-impl
	$(call reset)
	$(call cfile, $D/Malloc.c)
	$(call cfile, $D/Salloc.c)
	$(call cfile, $D/Srealloc.c)
	$(call cfile, $D/Realloc.c)
	$(call cfile, $D/botch.c)
	$(call cfile, $D/clear.c)
	$(call cfile, $D/st_alloc.c)
	$(call cfile, $D/std_alloc.c)
	$(call cfile, $D/No_Mem.c)
	$(call clibrary, $(LIBDIR)/liballoc.a)
	LIBALLOC := $o

	$(call reset)
	$(eval q := $D/alloc.h)
	$(call copyto, $(INCDIR)/alloc.h)
endef

$(eval $(build-liballoc-impl))

