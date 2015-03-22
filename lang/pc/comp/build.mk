D := lang/pc/comp

define build-pc-allocd-header
$2: $1 $D/make.allocd
	@echo ALLOCD $$@
	@mkdir -p $$(dir $$@)
	$(hide) $D/make.allocd < $$^ > $$@

$(eval CLEANABLES += $2)
$(eval $q: $2)
endef

define build-pc-next
$(eval CLEANABLES += $(OBJDIR)/$D/next.c)
$(OBJDIR)/$D/next.c: $D/make.next $1
	@echo NEXT $$@
	@mkdir -p $$(dir $$@)
	$(hide) $$^ > $$@
$(call cfile, $(OBJDIR)/$D/next.c)

$(foreach f, $1, $(call build-pc-allocd-header, \
	$f, $(OBJDIR)/$(basename $f).h))
endef

define build-pc-impl

$(call reset)
$(eval cflags += -I$(OBJDIR)/$D -I$D -DSTATIC=static)

$(call cfile, $D/LLlex.c)
$(call cfile, $D/LLmessage.c)
$(call cfile, $D/body.c)
$(call cfile, $D/chk_expr.c)
$(call cfile, $D/code.c)
$(call cfile, $D/cstoper.c)
$(call cfile, $D/def.c)
$(call cfile, $D/desig.c)
$(call cfile, $D/enter.c)
$(call cfile, $D/error.c)
$(call cfile, $D/idf.c)
$(call cfile, $D/input.c)
$(call cfile, $D/label.c)
$(call cfile, $D/lookup.c)
$(call cfile, $D/main.c)
$(call cfile, $D/misc.c)
$(call cfile, $D/node.c)
$(call cfile, $D/options.c)
$(call cfile, $D/progs.c)
$(call cfile, $D/readwrite.c)
$(call cfile, $D/scope.c)
$(call cfile, $D/stab.c)
$(call cfile, $D/tokenname.c)
$(call cfile, $D/type.c)
$(call cfile, $D/typequiv.c)

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
$(OBJDIR)/$D/parameters.h: $D/Parameters
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

$(call build-pc-next, \
		$D/def.H \
		$D/type.H \
		$D/node.H \
		$D/scope.H \
		$D/desig.H \
		$D/tmpvar.C \
		$D/casestat.C)

$(call build-pc-allocd-header, $D/tmpvar.C, $(OBJDIR)/$D/tmpvar.c)
$(call cfile, $(OBJDIR)/$D/tmpvar.c)

$(call build-pc-allocd-header, $D/casestat.C, $(OBJDIR)/$D/casestat.c)
$(call cfile, $(OBJDIR)/$D/casestat.c)

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
$(call cprogram, $(BINDIR)/em_pc)
$(call installto, $(PLATDEP)/em_pc)

$(call reset)
$(eval q := $D/em_pc.6)
$(call installto, $(INSDIR)/share/man/man6/em_pc.6)
endef

$(eval $(build-pc-impl))
