# Building tabgen.

define build-tabgen-impl
	$(call reset)
	$(call cfile, util/cmisc/tabgen.c)
	$(call cprogram, $(BINDIR)/tabgen)
	$(eval TABGEN := $o)
endef

$(eval $(build-tabgen-impl))

# Using tabgen.
#
#  $1 = input file
#
# Output file is compiled with cfile and queued.

define tabgen-impl
$(eval g := $(OBJDIR)/$(objdir)/$(strip $1).c)

$g: $1 $(TABGEN)
	@echo TABGEN $g
	@mkdir -p $(dir $g)
	$(hide) $(TABGEN) -f$(strip $1) > $g || $(RM) $g

$(eval CLEANABLES += $g)
$(call $2,$g)
endef

tabgen = $(eval $(call tabgen-impl,$1,cfile))
acktabgen = $(eval $(call tabgen-impl,$1,ackfile))

