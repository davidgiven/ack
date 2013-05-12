
define build-platform-headers
$(call reset)
$(eval q := $D/include/$(strip $1))
$(call installto, $(PLATIND)/$(PLATFORM)/include/$(strip $1))
$(eval PLATFORM_HEADERS_$(PLATFORM) += $q)
endef

define build-platform-impl
    $(call reset)
	$(eval q := $D/descr)
	$(call installto, $(PLATIND)/descr/$(PLATFORM))

	$(foreach f, $1, $(call build-platform-headers, $f))

	$(eval PLATFORM_$(PLATFORM) := \
			$(PLATFORM_HEADERS_$(PLATFORM)) \
			$(PLATDEP)/$(PLATFORM)/ncg)

    $(call build-as)
    $(call build-ncg)
endef

build-platform = $(eval $(call build-platform-impl, $1))


