D := lang/pcc

define pcc-cfile-impl
	$(call cfile, $1)
	$(call dependson, $(OBJDIR)/$D/pcc/external.h)
endef
pcc-cfile = $(eval $(pcc-cfile-impl))

define build-pcc-impl

$(eval E := $(OBJDIR)/$D/pcc)

$(eval pcc-cflags = \
	-Ilang/pcc \
	-I$E \
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
	-Dmach_$(PCCARCH) \
)

$(call reset)
$(eval cflags += $(pcc-cflags))

$(call pcc-cfile, $E/external.c)
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
$(call dependson, $E/cgram.h)

$(call pcc-cfile, lang/pcc/pcc/cc/ccom/pftn.c)
$(call dependson, $E/cgram.h)

$(call yacc, $E, lang/pcc/pcc/cc/ccom/cgram.y)

$(call flex, $E, lang/pcc/pcc/cc/ccom/scan.l)
$(call dependson, $E/cgram.h)

$(call cprogram, $(BINDIR)/$(PLATFORM)/pcc_ccom)
$(call installto, $(PLATDEP)/$(PLATFORM)/pcc_ccom)


$(call reset)
$(call rawfile, $E/y.tab.h)
$(call dependson, $E/y.tab.c)
$(call installto, $E/cgram.h)


$(call reset)
$(eval objdir := mkext)
$(eval cflags += $(pcc-cflags) -DMKEXT)
$(call cfile, lang/pcc/pcc/mip/mkext.c)
$(call cfile, lang/pcc/pcc/mip/common.c)
$(call cfile, lang/pcc/pcc/arch/$(PCCARCH)/table.c)
$(call cprogram, $E/mkext)
$(eval pcc_mkext := $o)

$E/external.h: $E/external.c
$E/external.c: $(pcc_mkext)
	@echo MKEXT
	@mkdir -p $$(dir $$@)
	$(hide) (cd $$(dir $$@) && $(pcc_mkext))

endef


build-pcc = $(eval $(build-pcc-impl))

