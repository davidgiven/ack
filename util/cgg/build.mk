D := util/cgg

define build-cgg-impl

$(call reset)
$(eval cflags += -I$D)

$(call yacc, $(OBJDIR)/$D, $D/bootgram.y)

$(call flex, $(OBJDIR)/$D, $D/bootlex.l)
$(call dependson, $(OBJDIR)/$D/y.tab.h)

$(call cfile, $D/main.c)

$(eval $q: $(INCDIR)/em_spec.h)

$(call rawfile, $(LIBEM_DATA))
$(call rawfile, $(LIBASSERT))
$(call rawfile, $(LIBSYSTEM))
$(call cprogram, $(BINDIR)/cgg)
$(eval CGG := $o)

endef

$(eval $(build-cgg-impl))
