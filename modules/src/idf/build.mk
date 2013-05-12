D := modules/src/idf

define build-libidf-impl
	$(call reset)
	$(eval q := $D/idf_pkg.body)
	$(call copyto, $(INCDIR)/idf_pkg.body)

	$(call reset)
	$(eval q := $D/idf_pkg.spec)
	$(call copyto, $(INCDIR)/idf_pkg.spec)
endef

$(eval $(build-libidf-impl))

