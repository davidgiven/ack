D := modules/src/em_mes

define build-em_mes-impl
	$(call reset)
	$(call cfile, $D/C_ms_err.c)
	$(call cfile, $D/C_ms_opt.c)
	$(call cfile, $D/C_ms_emx.c)
	$(call cfile, $D/C_ms_reg.c)
	$(call cfile, $D/C_ms_src.c)
	$(call cfile, $D/C_ms_flt.c)
	$(call cfile, $D/C_ms_com.c)
	$(call cfile, $D/C_ms_par.c)
	$(call cfile, $D/C_ms_ego.c)
	$(call cfile, $D/C_ms_gto.c)
	$(call cfile, $D/C_ms_stb.c)
	$(call cfile, $D/C_ms_std.c)

	$(eval $q: $(INCDIR)/em_codeEK.h)

	$(call clibrary, $(LIBDIR)/libem_mes.a)
	$(eval LIBEM_MES := $o)
endef

$(eval $(call build-em_mes-impl))


