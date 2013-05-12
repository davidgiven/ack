D := modules/src/input

define build-libinput-impl
	$(call reset)
	$(call cfile, $D/AtEoIF.c)
	$(call cfile, $D/AtEoIT.c)
	$(call clibrary, $(LIBDIR)/libinput.a)
	$(eval LIBINPUT := $o)

	$(call reset)
	$(eval q := $D/inp_pkg.body)
	$(call copyto, $(INCDIR)/inp_pkg.body)

	$(call reset)
	$(eval q := $D/inp_pkg.spec)
	$(call copyto, $(INCDIR)/inp_pkg.spec)
endef

$(eval $(build-libinput-impl))

