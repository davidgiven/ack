D := lang/m2/comp

define build-m2-allocd-header
$2: $1 $D/make.allocd
	@echo ALLOCD $$@
	@mkdir -p $$(dir $$@)
	$(hide) $D/make.allocd < $$^ > $$@

$(eval CLEANABLES += $2)
$(eval $q: $2)
endef

define build-m2-next
$(eval CLEANABLES += $(OBJDIR)/$D/next.c)
$(OBJDIR)/$D/next.c: $D/make.next $1
	@echo NEXT $$@
	@mkdir -p $$(dir $$@)
	$(hide) $$^ > $$@
$(call cfile, $(OBJDIR)/$D/next.c)

$(foreach f, $1, $(call build-m2-allocd-header, \
	$f, $(OBJDIR)/$(basename $f).h))
endef

define build-m2-impl

$(call reset)
$(eval cflags += -I$(OBJDIR)/$D -I$D)

$(call cfile, $D/LLlex.c)
$(call cfile, $D/LLmessage.c)
$(call cfile, $D/chk_expr.c)
$(call cfile, $D/code.c)
$(call cfile, $D/cstoper.c)
$(call cfile, $D/def.c)
$(call cfile, $D/defmodule.c)
$(call cfile, $D/desig.c)
$(call cfile, $D/enter.c)
$(call cfile, $D/error.c)
$(call cfile, $D/idf.c)
$(call cfile, $D/input.c)
$(call cfile, $D/lookup.c)
$(call cfile, $D/main.c)
$(call cfile, $D/misc.c)
$(call cfile, $D/node.c)
$(call cfile, $D/options.c)
$(call cfile, $D/stab.c)
$(call cfile, $D/tokenname.c)
$(call cfile, $D/type.c)
$(call cfile, $D/typequiv.c)
$(call cfile, $D/walk.c)

$(call llgen, $(OBJDIR)/$D, \
	$(OBJDIR)/$D/tokenfile.g \
	$D/program.g \
	$D/declar.g \
	$D/expression.g \
	$D/statement.g)

$(eval CLEANABLES += $(OBJDIR)/$D/tokenfile.g)
$(OBJDIR)/$D/tokenfile.g: $D/make.tokfile $D/tokenname.c
	@echo TOKENFILE $$@
	@mkdir -p $$(dir $$@)
	$(hide) sh $D/make.tokfile < $D/tokenname.c > $$@

$(call tabgen, $D/char.tab)

$(eval $q: $(OBJDIR)/$D/parameters.h)

$(eval CLEANABLES += $(OBJDIR)/$D/parameters.h)
$(OBJDIR)/$D/parameters.h: $D/BigPars
	@echo PARAMETERS $$@
	@mkdir -p $$(dir $$@)
	$(hide) echo '#ifndef PARAMETERS_H' > $$@
	$(hide) echo '#define PARAMETERS_H' >> $$@
	$(hide) grep -v '^!' < $$^ >> $$@
	$(hide) echo '#endif' >> $$@

$(eval CLEANABLES += $(OBJDIR)/$D/symbol2str.c)
$(OBJDIR)/$D/symbol2str.c: $D/make.tokcase $D/tokenname.c
	@echo TOKCASE $$@
	@mkdir -p $$(dir $$@)
	$(hide) $D/make.tokcase < $D/tokenname.c > $$@
$(call cfile, $(OBJDIR)/$D/symbol2str.c)

$(call build-m2-next, \
		$D/def.H \
		$D/type.H \
		$D/real.H \
		$D/node.H)

$(call build-m2-allocd-header, $D/tmpvar.C, $(OBJDIR)/$D/tmpvar.c)
$(call cfile, $(OBJDIR)/$D/tmpvar.c)

$(call build-m2-allocd-header, $D/casestat.C, $(OBJDIR)/$D/casestat.c)
$(call cfile, $(OBJDIR)/$D/casestat.c)

$(call build-m2-allocd-header, $D/scope.C, $(OBJDIR)/$D/scope.c)
$(call cfile, $(OBJDIR)/$D/scope.c)

$(eval $q: $(OBJDIR)/$D/Lpars.h)
$(eval $q: $(INCDIR)/idf_pkg.spec)
$(eval $q: $(INCDIR)/idf_pkg.body)
$(eval $q: $(INCDIR)/inp_pkg.spec)
$(eval $q: $(INCDIR)/inp_pkg.body)
$(eval $q: $(INCDIR)/alloc.h)
$(eval $q: $(INCDIR)/em_codeEK.h)
$(eval $q: $(INCDIR)/print.h)
$(eval $q: $(INCDIR)/system.h)

$(call rawfile, $(LIBEM_MES))
$(call rawfile, $(LIBEMK))
$(call rawfile, $(LIBEM_DATA))
$(call rawfile, $(LIBINPUT))
$(call rawfile, $(LIBASSERT))
$(call rawfile, $(LIBALLOC))
$(call rawfile, $(LIBFLT_ARITH))
$(call rawfile, $(LIBPRINT))
$(call rawfile, $(LIBSYSTEM))
$(call rawfile, $(LIBSTRING))
$(call cprogram, $(BINDIR)/em_m2)
$(call installto, $(PLATDEP)/em_m2)

$(call reset)
$(eval q := $D/em_m2.6)
$(call installto, $(INSDIR)/share/man/man6/em_m2.6)
endef

$(eval $(build-m2-impl))
