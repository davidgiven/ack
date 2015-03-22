D := lang/cem/cemcom.ansi

define build-cemcom-ansi-allocd-header
$(eval g := $(OBJDIR)/$D/$(strip $1).h)
$g: $D/$(strip $1).str $D/make.allocd
	@echo ALLOCD $$@
	@mkdir -p $$(dir $$@)
	$(hide) $D/make.allocd < $$^ > $$@

$(eval CLEANABLES += $g)
$(eval $q: $g)
endef

define build-cemcom-ansi-next
$(eval CLEANABLES += $(OBJDIR)/$D/next.c)
$(OBJDIR)/$D/next.c: $D/make.next $1
	@echo NEXT $$@
	@mkdir -p $$(dir $$@)
	$(hide) $$^ > $$@
$(call cfile, $(OBJDIR)/$D/next.c)

$(foreach f, $1, $(call build-cemcom-ansi-allocd-header, \
	$(basename $(notdir $f))))
endef

define build-cemcom-ansi-impl

$(call reset)
$(eval cflags += -I$(OBJDIR)/$D -I$D)

$(call cfile, $D/arith.c)
$(call dependson, $(INCDIR)/flt_arith.h)

$(call cfile, $D/blocks.c)
$(call dependson, $(INCDIR)/em_codeEK.h)

$(call cfile, $D/LLlex.c)
$(call cfile, $D/LLmessage.c)

$(call cfile, $D/ch3.c)
$(call cfile, $D/ch3bin.c)
$(call cfile, $D/ch3mon.c)
$(call cfile, $D/code.c)
$(call cfile, $D/conversion.c)
$(call cfile, $D/cstoper.c)
$(call cfile, $D/dataflow.c)
$(call cfile, $D/declarator.c)
$(call cfile, $D/decspecs.c)
$(call cfile, $D/domacro.c)
$(call cfile, $D/dumpidf.c)
$(call cfile, $D/error.c)
$(call cfile, $D/eval.c)
$(call cfile, $D/expr.c)
$(call cfile, $D/field.c)
$(call cfile, $D/fltcstoper.c)
$(call cfile, $D/idf.c)
$(call cfile, $D/init.c)
$(call cfile, $D/input.c)
$(call cfile, $D/l_comment.c)
$(call cfile, $D/l_ev_ord.c)
$(call cfile, $D/l_lint.c)
$(call cfile, $D/l_misc.c)
$(call cfile, $D/l_outdef.c)
$(call cfile, $D/l_states.c)
$(call cfile, $D/label.c)
$(call cfile, $D/main.c)
$(call cfile, $D/options.c)
$(call cfile, $D/pragma.c)
$(call cfile, $D/proto.c)
$(call cfile, $D/replace.c)
$(call cfile, $D/skip.c)
$(call cfile, $D/stab.c)
$(call cfile, $D/stack.c)
$(call cfile, $D/struct.c)
$(call cfile, $D/switch.c)
$(call cfile, $D/tokenname.c)
$(call cfile, $D/type.c)
$(call cfile, $D/util.c)

$(call llgen, $(OBJDIR)/$D, \
	$(OBJDIR)/$D/tokenfile.g \
	$D/program.g \
	$D/declar.g \
	$D/expression.g \
	$D/statement.g \
	$D/ival.g)

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
	$(hide) grep -v '^!' < $D/BigPars >> $$@
	$(hide) echo '#endif' >> $$@

$(eval CLEANABLES += $(OBJDIR)/$D/symbol2str.c)
$(OBJDIR)/$D/symbol2str.c: $D/make.tokcase $D/tokenname.c
	@echo TOKCASE $$@
	@mkdir -p $$(dir $$@)
	$(hide) $D/make.tokcase < $D/tokenname.c > $$@
$(call cfile, $(OBJDIR)/$D/symbol2str.c)

$(call build-cemcom-ansi-next, \
		$D/code.str \
		$D/declar.str \
		$D/def.str \
		$D/expr.str \
		$D/field.str \
		$D/estack.str \
		$D/util.str \
		$D/proto.str \
		$D/replace.str \
		$D/idf.str \
		$D/macro.str \
		$D/stack.str \
		$D/stmt.str \
		$D/struct.str \
		$D/switch.str \
		$D/type.str \
		$D/l_brace.str \
		$D/l_state.str \
		$D/l_outdef.str)

$(eval $q: $(OBJDIR)/$D/Lpars.h)

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
$(call cprogram, $(BINDIR)/cemcom.ansi)
$(call installto, $(PLATDEP)/em_cemcom.ansi)
$(eval CEMCOMANSI := $o)

$(call reset)
$(eval q := $D/cemcom.ansi.1)
$(call installto, $(INSDIR)/share/man/man1/cemcom.6)
endef

$(eval $(build-cemcom-ansi-impl))
