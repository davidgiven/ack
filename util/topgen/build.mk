D := util/topgen

# Rule to build topgen.

define build-topgen-impl
	$(call reset)
	$(eval cflags += -I$(OBJDIR)/$D -I$D)

	$(call cfile, $D/LLlex.c)
	$(call cfile, $D/hash.c)
	$(call cfile, $D/main.c)
	$(call cfile, $D/pattern.c)
	$(call cfile, $D/symtab.c)
	$(eval $q: $(OBJDIR)/$D/Lpars.h)

	$(call llgen, $(OBJDIR)/$D, $D/topgen.g)

	$(call file, $(LIBASSERT))
	$(call file, $(LIBPRINT))
	$(call file, $(LIBALLOC))
	$(call file, $(LIBSYSTEM))
	$(call file, $(LIBSTRING))

	$(call cprogram, $(BINDIR)/topgen)
	TOPGEN := $o
endef

$(eval $(build-topgen-impl))

# Rule to invoke to *use* LLgen.
#
#  $1: directory to put output files
#  $2: input files
#
# Output files are *not* compiled (gen.c is expected to be included by
# something).

define topgen-impl
$(eval CLEANABLES += $1/gen.h $1/gen.c)

$1/gen.h: $1/gen.c
$1/gen.c: $2 $(TOPGEN)
	@echo TOPGEN $$@
	@mkdir -p $$(dir $$@)
	$(hide) $(RM) $1/gen.h $1/gen.c
	$(hide) cd $$(dir $$@) && $(abspath $(TOPGEN)) $(abspath $2)

endef

topgen = $(eval $(call topgen-impl,$1,$2))
