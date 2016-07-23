cprogram {
	name = "esize",
	srcs = { "./esize.c" },
}

cprogram {
	name = "encode",
	srcs = { "./convert.c" },
	deps = {
		"h+emheaders",
		"modules+headers",
		"modules/src/alloc+lib",
		"modules/src/em_code+lib_k",
		"modules/src/print+lib",
		"modules/src/read_em+lib_ev",
		"modules/src/string+lib",
		"modules/src/system+lib",
		"util/data+em_data",
	}
}

installable {
	name = "pkg",
	map = {
		["$(PLATDEP)/em_encode"] = "+encode",
		["$(INSDIR)/bin/esize"] = "+esize",
		["$(PLATIND)/man/man1/esize.1"] = "./esize.1",
		["$(PLATIND)/man/man6/em_decode.6"] = "./em_decode.6"
	}
}

--[[
D := util/misc

define build-misc-impl
	$(call reset)
	$(call cfile, $D/esize.c)
	$(call cprogram, $(BINDIR)/esize)
	$(call installto, $(INSDIR)/bin/esize)

	$(call reset)
	$(eval q := $D/esize.1)
	$(call installto, $(INSDIR)/share/man/man1/esize.1)

	$(call reset)
	$(eval objdir := encode)
	$(call cfile, $D/convert.c)
	$(eval $q: $(INCDIR)/em_comp.h $(INCDIR)/em_codeEK.h)
	$(call rawfile, $(LIBREAD_EMEV))
	$(call rawfile, $(LIBEMK))
	$(call rawfile, $(LIBEM_DATA))
	$(call rawfile, $(LIBALLOC))
	$(call rawfile, $(LIBPRINT))
	$(call rawfile, $(LIBSTRING))
	$(call rawfile, $(LIBSYSTEM))
	$(call cprogram, $(BINDIR)/em_encode)
	$(call installto, $(PLATDEP)/em_encode)
	$(eval EM_ENCODE := $o)
	$(eval ACK_CORE_TOOLS += $o)

	$(call reset)
	$(eval objdir := decode)
	$(call cfile, $D/convert.c)
	$(eval $q: $(INCDIR)/em_comp.h $(INCDIR)/em_codeEK.h)
	$(call rawfile, $(LIBREAD_EMKV))
	$(call rawfile, $(LIBEME))
	$(call rawfile, $(LIBEM_DATA))
	$(call rawfile, $(LIBALLOC))
	$(call rawfile, $(LIBPRINT))
	$(call rawfile, $(LIBSTRING))
	$(call rawfile, $(LIBSYSTEM))
	$(call cprogram, $(BINDIR)/em_decode)
	$(call installto, $(PLATDEP)/em_decode)
	$(eval EM_DECODE := $o)

	$(call reset)
	$(eval q := $D/em_decode.6)
	$(call installto, $(INSDIR)/share/man/man6/em_decode.6)
endef

$(eval $(build-misc-impl))
--]]
