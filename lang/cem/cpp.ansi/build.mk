D := lang/cem/cpp.ansi

define build-cpp-ansi-allocd-header
$1: $2 $D/make.allocd
	@echo ALLOCD $1
	@mkdir -p $(dir $1)
	$(hide) $D/make.allocd < $2 > $1

$(eval CLEANABLES += $1)
endef

define build-cpp-ansi-tokfile
$(OBJDIR)/$D/tokenfile.g: $D/make.tokfile $D/tokenname.c
	@echo TOKENFILE $$@
	@mkdir -p $$(dir $$@)
	$(hide) sh $D/make.tokfile < $D/tokenname.c > $$@

$(eval CLEANABLES += $(OBJDIR)/$D/tokenfile.g)
endef

define build-cpp-ansi-tokcase
$(OBJDIR)/$D/symbol2str.c: $D/make.tokcase $D/tokenname.c
	@echo TOKCASE $$@
	@mkdir -p $$(dir $$@)
	$(hide) sh $D/make.tokcase < $D/tokenname.c > $$@

$(eval CLEANABLES += $(OBJDIR)/$D/symbol2str.c)
endef

define build-cpp-ansi-next
$(OBJDIR)/$D/next.c: $D/make.next $D/macro.str $D/replace.str
	@echo NEXT $$@
	@mkdir -p $$(dir $$@)
	$(hide) sh $D/make.next $D/macro.str $D/replace.str > $$@

$(eval CLEANABLES += $(OBJDIR)/$D/next.c)
endef

define build-cpp-ansi-impl
	$(eval $(call build-cpp-ansi-next))
	$(eval $(call build-cpp-ansi-tokcase))
	$(eval $(call build-cpp-ansi-tokfile))
	$(eval $(call build-cpp-ansi-allocd-header, \
	    $(OBJDIR)/$D/macro.h, $D/macro.str \
	))
	$(eval $(call build-cpp-ansi-allocd-header, \
	    $(OBJDIR)/$D/replace.h, $D/replace.str \
	))

	$(call reset)
	$(eval cflags += -I$(OBJDIR)/$D -I$D)

	$(call cfile, $D/LLlex.c)
	$(call cfile, $D/LLmessage.c)
	$(call cfile, $D/ch3bin.c)
	$(call cfile, $D/ch3mon.c)
	$(call cfile, $D/domacro.c)
	$(call cfile, $D/error.c)
	$(call cfile, $D/idf.c)
	$(call cfile, $D/init.c)
	$(call cfile, $D/input.c)
	$(call cfile, $D/main.c)
	$(call cfile, $D/options.c)
	$(call cfile, $D/preprocess.c)
	$(call cfile, $D/replace.c)
	$(call cfile, $D/skip.c)
	$(call cfile, $D/tokenname.c)
	$(call cfile, $D/expr.c)
	$(call cfile, $(OBJDIR)/$D/symbol2str.c)
	$(call cfile, $(OBJDIR)/$D/next.c)

	$(call llgen, $(OBJDIR)/$D, $(OBJDIR)/$D/tokenfile.g $D/expression.g)

	$(call rawfile, $(LIBINPUT))
	$(call rawfile, $(LIBASSERT))
	$(call rawfile, $(LIBALLOC))
	$(call rawfile, $(LIBPRINT))
	$(call rawfile, $(LIBSYSTEM))
	$(call rawfile, $(LIBSTRING))

	$(call tabgen, $D/char.tab)

	$(eval $q: \
		$(OBJDIR)/$D/macro.h \
		$(OBJDIR)/$D/Lpars.h \
		$(INCDIR)/alloc.h \
		$(INCDIR)/inp_pkg.spec \
		$(INCDIR)/idf_pkg.spec \
		$(OBJDIR)/$D/replace.h \
		$(INCDIR)/system.h \
		$(INCDIR)/inp_pkg.body \
		$(INCDIR)/inp_pkg.spec \
		$(INCDIR)/idf_pkg.body)

	$(call cprogram, $(BINDIR)/cpp.ansi)
	$(call installto, $(PLATDEP)/cpp.ansi)
	$(eval CPPANSI := $o)

	$(call reset)
	$(eval q := $D/ncpp.6)
	$(call installto, $(INSDIR)/share/man/man6/cpp.ansi.6)
endef

$(eval $(build-cpp-ansi-impl))
