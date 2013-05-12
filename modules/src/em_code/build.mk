D := modules/src/em_code

# $1 = capital letter for library specialisation (E or K)
# $2 = lowercase letter for library specialisation (e or k)
# $3 = cflags that specalise this library

define build-em_code-impl
	$(call reset)
	$(eval cflags += $3)
	$(eval objdir := $D/$1)
	$(call cfile, $D/bhcst.c)
	$(call cfile, $D/bhdlb.c)
	$(call cfile, $D/bhdnam.c)
	$(call cfile, $D/bhfcon.c)
	$(call cfile, $D/bhicon.c)
	$(call cfile, $D/bhilb.c)
	$(call cfile, $D/bhpnam.c)
	$(call cfile, $D/bhucon.c)
	$(call cfile, $D/crcst.c)
	$(call cfile, $D/crdlb.c)
	$(call cfile, $D/crdnam.c)
	$(call cfile, $D/crxcon.c)
	$(call cfile, $D/crilb.c)
	$(call cfile, $D/crpnam.c)
	$(call cfile, $D/crscon.c)
	$(call cfile, $D/cst.c)
	$(call cfile, $D/dfdlb.c)
	$(call cfile, $D/dfdnam.c)
	$(call cfile, $D/dfilb.c)
	$(call cfile, $D/dlb.c)
	$(call cfile, $D/dnam.c)
	$(call cfile, $D/end.c)
	$(call cfile, $D/endarg.c)
	$(call cfile, $D/exc.c)
	$(call cfile, $D/fcon.c)
	$(call cfile, $D/getid.c)
	$(call cfile, $D/icon.c)
	$(call cfile, $D/ilb.c)
	$(call cfile, $D/insert.c)
	$(call cfile, $D/internerr.c)
	$(call cfile, $D/msend.c)
	$(call cfile, $D/op.c)
	$(call cfile, $D/opcst.c)
	$(call cfile, $D/opdlb.c)
	$(call cfile, $D/opdnam.c)
	$(call cfile, $D/opilb.c)
	$(call cfile, $D/opnarg.c)
	$(call cfile, $D/oppnam.c)
	$(call cfile, $D/pnam.c)
	$(call cfile, $D/pro.c)
	$(call cfile, $D/pronarg.c)
	$(call cfile, $D/msstart.c)
	$(call cfile, $D/psdlb.c)
	$(call cfile, $D/psdnam.c)
	$(call cfile, $D/pspnam.c)
	$(call cfile, $D/scon.c)
	$(call cfile, $D/ucon.c)
	$(call cfile, $D/C_out.c)
	$(call cfile, $D/failed.c)
	$(call cfile, $D/em.c)

	$(eval $q: $(INCDIR)/em_codeEK.h)

	$(call clibrary, $(LIBDIR)/libem$2.a)
	$(eval LIBEM$1 := $o)
endef

define build-em_code-header-impl
$(OBJDIR)/$D/em_codeEK.h: $D/make.em.gen $D/em.nogen h/em_table
	@echo MAKE_EM_GEN $$@
	@mkdir -p $$(dir $$@)
	$(hide) $D/make.em.gen h/em_table > $$@
	$(hide) cat $D/em.nogen >> $$@

$(call reset)
$(eval q := $(OBJDIR)/$D/em_codeEK.h)
$(eval CLEANABLES += $q)
$(call copyto, $(INCDIR)/em_codeEK.h)

$(eval CLEANABLES += $o)
$(eval $o: $(INCDIR)/em_pseu.h)

endef

$(eval $(call build-em_code-header-impl))
$(eval $(call build-em_code-impl,E,e, -DREADABLE_EM))
$(eval $(call build-em_code-impl,K,k, ))


