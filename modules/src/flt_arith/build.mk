D := modules/src/flt_arith

define build-libflt_arith-impl
	$(call reset)
	$(call cfile, $D/flt_ar2flt.c)
	$(call cfile, $D/flt_div.c)
	$(call cfile, $D/flt_flt2ar.c)
	$(call cfile, $D/flt_modf.c)
	$(call cfile, $D/flt_str2fl.c)
	$(call cfile, $D/flt_cmp.c)
	$(call cfile, $D/flt_add.c)
	$(call cfile, $D/b64_add.c)
	$(call cfile, $D/flt_mul.c)
	$(call cfile, $D/flt_nrm.c)
	$(call cfile, $D/b64_sft.c)
	$(call cfile, $D/flt_umin.c)
	$(call cfile, $D/flt_chk.c)
	$(call cfile, $D/split.c)
	$(call cfile, $D/ucmp.c)
	$(call clibrary, $(LIBDIR)/libflt_arith.a)
	LIBFLT_ARITH := $o

	$(call reset)
	$(eval q := $D/flt_arith.h)
	$(call copyto, $(INCDIR)/flt_arith.h)
endef

$(eval $(build-libflt_arith-impl))

