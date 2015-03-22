D := util/ncgg

define build-ncgg-impl

$(call reset)
$(eval cflags += -I$D)

$(call yacc, $(OBJDIR)/$D, $D/cgg.y)

$(call flex, $(OBJDIR)/$D, $D/scan.l)
$(call dependson, $(OBJDIR)/$D/y.tab.h)

$(call cfile, $D/subr.c)
$(call cfile, $D/main.c)
$(call cfile, $D/coerc.c)
$(call cfile, $D/error.c)
$(call cfile, $D/emlookup.c)
$(call cfile, $D/expr.c)
$(call cfile, $D/instruct.c)
$(call cfile, $D/iocc.c)
$(call cfile, $D/lookup.c)
$(call cfile, $D/output.c)
$(call cfile, $D/set.c)
$(call cfile, $D/strlookup.c)
$(call cfile, $D/var.c)
$(call cfile, $D/hall.c)

$(eval CLEANABLES += $(OBJDIR)/$D/enterkeyw.c)
$(OBJDIR)/$D/enterkeyw.c: $D/cvtkeywords $D/keywords
	@echo KEYWORDS $$@
	@mkdir -p $$(dir $$@)
	$(hide) cd $$(dir $$@) && sh $(abspath $D/cvtkeywords) $(abspath $D/keywords)
$(call cfile, $(OBJDIR)/$D/enterkeyw.c)

$(eval $q: $(INCDIR)/em_spec.h)

$(call rawfile, $(LIBEM_DATA))
$(call cprogram, $(BINDIR)/ncgg)
$(eval NCGG := $o)

endef

$(eval $(build-ncgg-impl))
