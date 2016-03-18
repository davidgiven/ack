D := lang/pcc

define pcc-cfile-impl
	$(call cfile, $1)
	$(call dependson, $(OBJDIR)/$D/external.h)
endef
pcc-cfile = $(eval $(pcc-cfile-impl))

define build-pcc-impl

$(eval pcc-cflags = \
	-Ilang/pcc \
	-I$(OBJDIR)/$D \
	-Ilang/pcc/pcc/common \
	-Ilang/pcc/pcc/mip \
	-Ilang/pcc/pcc/cc/ccom \
	-Ilang/pcc/pcc/arch/$(PCCARCH) \
	-D_DEFAULT_SOURCE \
	-DGCC_COMPAT \
	-DPCC_DEBUG \
	-DNATIVE_FLOATING_POINT \
	-D_ISOC99_SOURCE \
	-Dos_ack \
	-Dmach_$(PCCMACH) \
)

$(call reset)
$(eval cflags += $(pcc-cflags))

$(call pcc-cfile, $(OBJDIR)/$D/external.c)
$(call pcc-cfile, lang/pcc/pcc/arch/$(PCCARCH)/code.c)
$(call pcc-cfile, lang/pcc/pcc/arch/$(PCCARCH)/local.c)
$(call pcc-cfile, lang/pcc/pcc/arch/$(PCCARCH)/local2.c)
$(call pcc-cfile, lang/pcc/pcc/arch/$(PCCARCH)/order.c)
$(call pcc-cfile, lang/pcc/pcc/arch/$(PCCARCH)/table.c)
$(call pcc-cfile, lang/pcc/pcc/cc/ccom/builtins.c)
$(call pcc-cfile, lang/pcc/pcc/cc/ccom/dwarf.c)
$(call pcc-cfile, lang/pcc/pcc/cc/ccom/init.c)
$(call pcc-cfile, lang/pcc/pcc/cc/ccom/inline.c)
$(call pcc-cfile, lang/pcc/pcc/cc/ccom/main.c)
$(call pcc-cfile, lang/pcc/pcc/cc/ccom/optim.c)
$(call pcc-cfile, lang/pcc/pcc/cc/ccom/stabs.c)
$(call pcc-cfile, lang/pcc/pcc/cc/ccom/symtabs.c)
$(call pcc-cfile, lang/pcc/pcc/cc/ccom/trees.c)
$(call pcc-cfile, lang/pcc/pcc/common/compat.c)
$(call pcc-cfile, lang/pcc/pcc/common/unicode.c)
$(call pcc-cfile, lang/pcc/pcc/mip/common.c)
$(call pcc-cfile, lang/pcc/pcc/mip/match.c)
$(call pcc-cfile, lang/pcc/pcc/mip/optim2.c)
$(call pcc-cfile, lang/pcc/pcc/mip/reader.c)
$(call pcc-cfile, lang/pcc/pcc/mip/regs.c)

$(call pcc-cfile, lang/pcc/pcc/cc/ccom/gcc_compat.c)
$(call dependson, $(OBJDIR)/$D/cgram.h)

$(call pcc-cfile, lang/pcc/pcc/cc/ccom/pftn.c)
$(call dependson, $(OBJDIR)/$D/cgram.h)

$(call yacc, $(OBJDIR)/$D, lang/pcc/pcc/cc/ccom/cgram.y)

$(call flex, $(OBJDIR)/$D, lang/pcc/pcc/cc/ccom/scan.l)
$(call dependson, $(OBJDIR)/$D/cgram.h)

$(call cprogram, $(BINDIR)/$(PLATFORM)/pcc_ccom)
$(call installto, $(PLATDEP)/$(PLATFORM)/pcc_ccom)


$(call reset)
$(call rawfile, $(OBJDIR)/$D/y.tab.h)
$(call installto, $(OBJDIR)/$D/cgram.h)


$(call reset)
$(eval cflags += $(pcc-cflags))


$(call reset)
$(eval objdir := mkext)
$(eval cflags += $(pcc-cflags) -DMKEXT)
$(call cfile, lang/pcc/pcc/mip/mkext.c)
$(call cfile, lang/pcc/pcc/mip/common.c)
$(call cfile, lang/pcc/pcc/arch/$(PCCARCH)/table.c)
$(call cprogram, $(OBJDIR)/$D/mkext)
$(eval pcc_mkext := $o)

$(OBJDIR)/$D/external.h: $(OBJDIR)/$D/external.c
$(OBJDIR)/$D/external.c: $(pcc_mkext)
	@echo MKEXT
	@mkdir -p $$(dir $$@)
	$(hide) (cd $$(dir $$@) && $(pcc_mkext))

endef


build-pcc = $(eval $(build-pcc-impl))

