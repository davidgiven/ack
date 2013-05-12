
define build-simple-tool-impl
	$(call reset)
	$(call cfile, util/amisc/$1.c)
	$(call file, $(LIBOBJECT))
	$(call cprogram, $(BINDIR)/$1)
	$(eval INSTALLABLES += $o)
	$(call installto, $(INSDIR)/bin/$1)

	$(call reset)
	$(eval q := util/amisc/$1.1)
	$(call installto, $(INSDIR)/share/man/man1/$1.1)
endef

build-simple-tool = $(eval $(build-simple-tool-impl))

$(call build-simple-tool,anm)
$(call build-simple-tool,ashow)
$(call build-simple-tool,asize)
$(call build-simple-tool,aslod)
$(call build-simple-tool,astrip)

