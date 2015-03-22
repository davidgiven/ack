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

$(eval g := $(OBJDIR)/$D/tokentab.h)
$(eval $q: $g)
$(eval CLEANABLES += $g)
$g: $D/maketokentab $(OBJDIR)/$D/Lpars.h
	@echo TOKENTAB $$@
	@mkdir -p $$(dir $$@)
	$(hide) cd $(OBJDIR)/$D && $(abspath $$^)

$(eval $q: $(OBJDIR)/$D/Lpars.h)
$(eval $q: $(INCDIR)/print.h)

$(call rawfile, $(LIBEM_MES))
$(call rawfile, $(LIBEMK))
$(call rawfile, $(LIBEM_DATA))
$(call rawfile, $(LIBALLOC))
$(call rawfile, $(LIBPRINT))
$(call rawfile, $(LIBSTRING))
$(call rawfile, $(LIBSYSTEM))
$(call cprogram, $(BINDIR)/em_bem)
$(call installto, $(PLATDEP)/em_bem)

endef

$(eval $(build-bem-impl))
