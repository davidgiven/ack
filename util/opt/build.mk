D := util/opt

define build-opt-mktab-impl

$(call reset)
$(eval cflags += -I$D)

$(call yacc, $(OBJDIR)/$D, $D/mktab.y)

$(call flex, $(OBJDIR)/$D, $D/scan.l)
$(call dependson, $(OBJDIR)/$D/y.tab.h)

$(call rawfile, $(LIBEM_DATA))
$(call rawfile, -lfl)
$(call cprogram, $(OBJDIR)/$D/mktab)

endef

.PHONY: -lfl
-lfl:

define build-opt-impl

$(call reset)
$(eval objdir := $1)

$(eval cflags += -I$D $2)
$(call cfile, $D/main.c)
$(call cfile, $D/getline.c)
$(call cfile, $D/lookup.c)
$(call cfile, $D/var.c)
$(call cfile, $D/process.c)
$(call cfile, $D/backward.c)
$(call cfile, $D/util.c)
$(call cfile, $D/alloc.c)
$(call cfile, $D/putline.c)
$(call cfile, $D/cleanup.c)
$(call cfile, $D/peephole.c)
$(call cfile, $D/flow.c)
$(call cfile, $D/tes.c)
$(call cfile, $D/reg.c)

$(eval g := $(OBJDIR)/$D/$(objdir)/pop_push.c)
$(eval CLEANABLES += $g)
$g: $D/pop_push.awk h/em_table
	@echo POP_PUSH $$@
	@mkdir -p $$(dir $$@)
	$(hide) awk -f $D/pop_push.awk < h/em_table > $$@
$(call cfile, $g)

$(eval g := $(OBJDIR)/$D/$(objdir)/pattern.c)
$(eval CLEANABLES += $g)
$g: $(OBJDIR)/$D/mktab $D/patterns $(BINDIR)/cpp.ansi
	@echo PATTERNS $$@
	@mkdir -p $$(dir $$@)
	$(hide) $(BINDIR)/cpp.ansi < $D/patterns | $(OBJDIR)/$D/mktab > $$@
$(call cfile, $g)

$(call rawfile, $(LIBEM_DATA))
$(call rawfile, $(LIBASSERT))
$(call rawfile, $(LIBPRINT))
$(call rawfile, $(LIBALLOC))
$(call rawfile, $(LIBSYSTEM))
$(call rawfile, $(LIBSTRING))

$(eval $q: $(INCDIR)/em_spec.h)

$(call cprogram, $(BINDIR)/$(strip $1))
$(call installto, $(PLATDEP)/$(strip $1))
$(eval ACK_CORE_TOOLS += $o)

endef

define build-opt-manpage-impl

$(call reset)
$(eval q := $D/em_opt.6)
$(call installto, $(INSDIR)/share/man/man6/em_opt.6)

endef

$(eval $(build-opt-mktab-impl))
$(eval $(call build-opt-impl, em_opt,))
$(eval $(call build-opt-impl, em_opt2, -DGLOBAL_OPT))
$(eval $(build-opt-manpage-impl))
