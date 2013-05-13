define build-top-impl
	$(call reset)
	$(eval cflags += -I$(OBJDIR)/$(PLATFORM)/top)

	$(call cfile, mach/proto/top/queue.c)
	$(call cfile, mach/proto/top/top.c)
	$(eval $q: $(OBJDIR)/$(PLATFORM)/top/gen.h)

	$(call topgen, $(OBJDIR)/$(PLATFORM)/top, mach/$(ARCH)/top/table)

	$(call cprogram, $(OBJDIR)/$(PLATFORM)/top/top)
	$(call installto, $(PLATDEP)/$(PLATFORM)/top)
endef

build-top = $(eval $(build-top-impl))
