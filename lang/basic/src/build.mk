D := lang/basic/src

define build-bem-impl

$(call reset)
$(eval cflags += -I$(OBJDIR)/$D -I$D)

$(call cfile, $D/bem.c)
$(call cfile, $D/symbols.c)
$(call cfile, $D/initialize.c)
$(call cfile, $D/compile.c)
$(call cfile, $D/parsepar.c)
$(call cfile, $D/gencode.c)
$(call cfile, $D/util.c)
$(call cfile, $D/graph.c)
$(call cfile, $D/eval.c)
$(call cfile, $D/func.c)

$(call llgen, $(OBJDIR)/$D, $D/basic.g)

$(eval g := $(OBJDIR)/$D/token.h)
$(eval CLEANABLES += $g)
$g: $D/maketokentab $(OBJDIR)/$D/Lpars.h
	@echo TOKENTAB $$@
	@mkdir -p $$(dir $$@)
	$(hide) cd $(OBJDIR)/$D && $(abspath $$^)

$(eval $q: $(OBJDIR)/$D/token.h)
$(eval $q: $(OBJDIR)/$D/Lpars.h)
$(eval $q: $(INCDIR)/print.h)

$(call file, $(LIBEM_MES))
$(call file, $(LIBEMK))
$(call file, $(LIBEM_DATA))
$(call file, $(LIBALLOC))
$(call file, $(LIBPRINT))
$(call file, $(LIBSTRING))
$(call file, $(LIBSYSTEM))
$(call cprogram, $(BINDIR)/em_bem)
$(call installto, $(PLATDEP)/em_bem)

endef

$(eval $(build-bem-impl))
