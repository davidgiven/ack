define build-ncg-impl

$(call reset)

$(eval cflags += -Imach/$(ARCH)/ncg -I$(OBJDIR)/$D -Imach/proto/ncg)
$(eval objdir := $(ARCH))

$(call cfile, mach/proto/ncg/codegen.c)
$(call cfile, mach/proto/ncg/compute.c)
$(call cfile, mach/proto/ncg/equiv.c)
$(call cfile, mach/proto/ncg/fillem.c)
$(call cfile, mach/proto/ncg/gencode.c)
$(call cfile, mach/proto/ncg/glosym.c)
$(call cfile, mach/proto/ncg/label.c)
$(call cfile, mach/proto/ncg/main.c)
$(call cfile, mach/proto/ncg/move.c)
$(call cfile, mach/proto/ncg/nextem.c)
$(call cfile, mach/proto/ncg/reg.c)
$(call cfile, mach/proto/ncg/regvar.c)
$(call cfile, mach/proto/ncg/salloc.c)
$(call cfile, mach/proto/ncg/state.c)
$(call cfile, mach/proto/ncg/subr.c)
$(call cfile, mach/proto/ncg/var.c)

$(eval $q: $(OBJDIR)/$D/tables.h)
$(eval CLEANABLES += $(OBJDIR)/$D/tables.h $(OBJDIR)/$D/table.c)
$(OBJDIR)/$D/tables.c: $(OBJDIR)/$D/tables.h
$(OBJDIR)/$D/tables.h: $(NCGG) $(CPPANSI) mach/$(ARCH)/ncg/table
	@echo NCGG $$@
	@mkdir -p $$(dir $$@)
	$(hide) cd $$(dir $$@) && \
		$(abspath $(CPPANSI)) -I$(abspath mach/$(ARCH)/ncg) $(abspath mach/$(ARCH)/ncg/table) | $(abspath $(NCGG))
	$(hide) mv $(OBJDIR)/$D/tables.H $(OBJDIR)/$D/tables.h

$(call cfile, $(OBJDIR)/$D/tables.c)

$(call file, $(LIBEM_DATA))
$(call file, $(LIBFLT_ARITH))

$(call cprogram, $(BINDIR)/$(PLATFORM)/ncg)
$(call installto, $(PLATDEP)/$(PLATFORM)/ncg)

endef

build-ncg = $(eval $(build-ncg-impl))

