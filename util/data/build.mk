D := util/data

define util-data-impl

$(eval g := \
	$(INCDIR)/em_spec.h \
	$(INCDIR)/em_pseu.h \
	$(INCDIR)/em_mnem.h \
	$(OBJDIR)/$D/em_flag.c \
	$(OBJDIR)/$D/em_pseu.c \
	$(OBJDIR)/$D/em_mnem.c)

$(eval CLEANABLES += $g)
$(wordlist 2, $(words $g), $g): $(firstword $g)
$(firstword $g): $D/new_table h/em_table
	@echo DATA
	@mkdir -p $(dir $g)
	$(hide) $D/new_table h/em_table $(INCDIR) $(OBJDIR)/$D

$(call reset)
$(call cfile, $(OBJDIR)/$D/em_flag.c)
$(call cfile, $(OBJDIR)/$D/em_pseu.c)
$(call cfile, $(OBJDIR)/$D/em_mnem.c)
$(call cfile, $D/em_ptyp.c)
$(call clibrary, $(LIBDIR)/libem_data.a)
$(eval LIBEM_DATA := $q)

endef

$(eval $(util-data-impl))
