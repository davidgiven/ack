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

	$(call rawfile, $(LIBOBJECT))
    $(call cprogram, $(BINDIR)/$(PLATFORM)/as)
    $(call installto, $(PLATDEP)/$(PLATFORM)/as)

	$(call reset)
    $(eval cflags += -Imach/$(ARCH)/as -I$(OBJDIR)/$D)
    $(eval objdir := $(ARCH))
	$(call cppfile, mach/proto/as/comm2.y)
	$(call installto, $(OBJDIR)/$D/preprocessed-comm2.y)

    $(call reset)
    $(call rawfile, man/$(ARCH)_as.6)
    $(call installto, $(INSDIR)/share/man/man6/$(ARCH)_as.6)
endef

build-as = $(eval $(build-as-impl))

