D := modules/src/read_em

define build-read_em-headers-impl

$(eval g := $(OBJDIR)/$D/C_mnem_narg.h)
$(eval CLEANABLES += $g)
$g: $D/m_C_mnem_na h/em_table
	@echo M_C_MNEM_NA $g
	@mkdir -p $(dir $g)
	$(hide) cd $D && $(abspath $D/m_C_mnem_na) $(abspath h/em_table) > $(abspath $g)

$(eval g := $(OBJDIR)/$D/C_mnem.h)
$(eval CLEANABLES += $g)
$g: $D/m_C_mnem h/em_table
	@echo M_C_MNEM $g
	@mkdir -p $(dir $g)
	$(hide) cd $D && $(abspath $D/m_C_mnem) $(abspath h/em_table) > $(abspath $g)

endef

define build-read_em-impl

$(call reset)
$(eval objdir := emev)
$(eval cflags += -I$(OBJDIR)/$D -DPRIVATE=static -DEXPORT= -DNDEBUG -DCHECKING)
$(call cfile, $D/EM_vars.c)
$(call cfile, $D/read_em.c)
$(call cfile, $D/mkcalls.c)
$(eval $q: $(OBJDIR)/$D/C_mnem_narg.h)
$(eval $q: $(OBJDIR)/$D/C_mnem.h)
$(call clibrary, $(LIBDIR)/lib_read_emeV.a)
$(eval LIBREAD_EMEV := $q)

$(call reset)
$(eval objdir := emkv)
$(eval cflags += -I$(OBJDIR)/$D -DPRIVATE=static -DEXPORT= -DNDEBUG -DCHECKING -DCOMPACT)
$(call cfile, $D/EM_vars.c)
$(call cfile, $D/read_em.c)
$(call cfile, $D/mkcalls.c)
$(eval $q: $(OBJDIR)/$D/C_mnem_narg.h)
$(eval $q: $(OBJDIR)/$D/C_mnem.h)
$(call clibrary, $(LIBDIR)/lib_read_emkV.a)
$(eval LIBREAD_EMKV := $q)

$(call reset)
$(eval q := $D/em_comp.h)
$(call installto, $(INCDIR)/em_comp.h)

endef

$(eval $(call build-read_em-headers-impl))
$(eval $(call build-read_em-impl))


