D := util/opt

define build-opt-mktab-impl

$(call reset)
$(eval cflags += -I$D)

$(call yacc, $(OBJDIR)/$D, $D/mktab.y)

$(call flex, $(OBJDIR)/$D, $D/scan.l)
$(call dependson, $(OBJDIR)/$D/y.tab.h)

$(call file, $(LIBEM_DATA))
$(call file, -lfl)
$(call cprogram, $D/mktab)

endef

define build-opt-impl

$(call reset)
$(eval cflags += -I$D)
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

$(eval CLEANABLES += $(OBJDIR)/$D/pop_push.c)
$(OBJDIR)/$D/pop_push.c: $D/pop_push.awk h/em_table
	@echo POP_PUSH $$@
	@mkdir -p $$(dir $$@)
	$(hide) awk -f $D/pop_push.awk < h/em_table > $$@
$(call cfile, $(OBJDIR)/$D/pop_push.c)

$(eval CLEANABLES += $(OBJDIR)/$D/pattern.c)
$(OBJDIR)/$D/pattern.c: $D/mktab $(OBJDIR)/$D/patterns
	@echo PATTERNS $$@
	@mkdir -p $$(dir $$@)
	$(hide) $D/mktab < $(OBJDIR)/$D/patterns > $$@
$(call cfile, $(OBJDIR)/$D/pattern.c)

$(eval CLEANABLES += $(OBJDIR)/$D/patterns)
$(OBJDIR)/$D/patterns: $(CPPANSI) $D/patterns
	@echo PREPROCESS $$@
	@mkdir -p $$(dir $$@)
	$(hide) $(CPPANSI) < $D/patterns > $$@

$(call file, $(LIBEM_DATA))
$(call file, $(LIBASSERT))
$(call file, $(LIBPRINT))
$(call file, $(LIBALLOC))
$(call file, $(LIBSYSTEM))
$(call file, $(LIBSTRING))

$(eval $q: $(INCDIR)/em_spec.h)

$(call cprogram, $(BINDIR)/em_opt)
$(call installto, $(PLATDEP)/em_opt)
$(eval EM_OPT := $o)
$(eval ACK_CORE_TOOLS += $o)

$(call reset)
$(eval q := $D/em_opt.6)
$(call installto, $(INSDIR)/share/man/man6/em_opt.6)

endef

$(eval $(build-opt-mktab-impl))
$(eval $(build-opt-impl))
