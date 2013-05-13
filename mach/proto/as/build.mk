define build-as-impl
    $(call reset)

    $(eval cflags += -Imach/$(ARCH)/as -I$(OBJDIR)/$D)
    $(eval objdir := $(ARCH))

    $(call cfile, mach/proto/as/comm3.c)
    $(call dependson, $(OBJDIR)/$D/y.tab.h)

    $(call cfile, mach/proto/as/comm4.c)
    $(call dependson, $(OBJDIR)/$D/y.tab.h)

    $(call cfile, mach/proto/as/comm5.c)
    $(call dependson, $(OBJDIR)/$D/y.tab.h)

    $(call cfile, mach/proto/as/comm6.c)
    $(call dependson, $(OBJDIR)/$D/y.tab.h)

    $(call cfile, mach/proto/as/comm7.c)
    $(call dependson, $(OBJDIR)/$D/y.tab.h)

    $(call cfile, mach/proto/as/comm8.c)
    $(call dependson, $(OBJDIR)/$D/y.tab.h)

	$(call yacc, $(OBJDIR)/$D, $(OBJDIR)/$D/preprocessed-comm2.y)

$(eval CLEANABLES += $(OBJDIR)/$D/preprocessed-comm2.y)
$(OBJDIR)/$D/preprocessed-comm2.y: mach/proto/as/comm2.y $(CPPANSI)
	@echo PREPROCESS $$@
	@mkdir -p $$(dir $$@)
	$(hide) $(CPPANSI) -P \
		-Imach/$(ARCH)/as \
		-Imach/proto/as \
		-Ih \
		mach/proto/as/comm2.y > $$@

	$(call file, $(LIBOBJECT))
    $(call cprogram, $(BINDIR)/$(ARCH)/as)
    $(call installto, $(PLATDEP)/$(ARCH)/as)
endef

build-as = $(eval $(build-as-impl))

