define build-cg-impl

$(call reset)

$(eval cflags += -Imach/$(ARCH)/cg -I$(OBJDIR)/$D -Imach/proto/cg)
$(eval objdir := $(ARCH))

$(call cfile, mach/proto/cg/codegen.c)
$(call cfile, mach/proto/cg/compute.c)
$(call cfile, mach/proto/cg/equiv.c)
$(call cfile, mach/proto/cg/fillem.c)
$(call cfile, mach/proto/cg/gencode.c)
$(call cfile, mach/proto/cg/glosym.c)
$(call cfile, mach/proto/cg/main.c)
$(call cfile, mach/proto/cg/move.c)
$(call cfile, mach/proto/cg/nextem.c)
$(call cfile, mach/proto/cg/reg.c)
$(call cfile, mach/proto/cg/regvar.c)
$(call cfile, mach/proto/cg/salloc.c)
$(call cfile, mach/proto/cg/state.c)
$(call cfile, mach/proto/cg/subr.c)
$(call cfile, mach/proto/cg/var.c)

$(eval $q: $(OBJDIR)/$D/tables.h)
$(eval CLEANABLES += $(OBJDIR)/$D/tables.h $(OBJDIR)/$D/tables.c)
$(OBJDIR)/$D/tables.c: $(OBJDIR)/$D/tables.h
$(OBJDIR)/$D/tables.h: $(CGG) $(CPPANSI) mach/$(ARCH)/cg/table
	@echo CGG $$@
	@mkdir -p $$(dir $$@)
	$(hide) cd $$(dir $$@) && \
		$(abspath $(CPPANSI)) -I$(abspath mach/$(ARCH)/cg) $(abspath mach/$(ARCH)/cg/table) | $(abspath $(CGG))

$(call cfile, $(OBJDIR)/$D/tables.c)

$(eval $q: $(INCDIR)/flt_arith.h)

$(call rawfile, $(LIBEM_DATA))
$(call rawfile, $(LIBFLT_ARITH))

$(call cprogram, $(BINDIR)/$(PLATFORM)/cg)
$(call installto, $(PLATDEP)/$(PLATFORM)/cg)

endef

build-cg = $(eval $(build-cg-impl))

