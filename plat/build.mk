
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

	$(eval PLATFORM_$(PLATFORM) := \
			$(PLATFORM_HEADERS_$(PLATFORM)) \
			$(PLATDEP)/$(PLATFORM)/ncg)

	$(foreach f, $(platform-headers), $(call build-platform-headers, $f))

	$(call reset)
	$(foreach f, $(platform-libsys), $(call ackfile, $D/libsys/$f))
	$(call acklibrary, $(LIBDIR)/$(PLATFORM)/libsys.a)
	$(call installto, $(PLATIND)/$(PLATFORM)/libsys.a)

    $(call build-as)
    $(call build-ncg)

    $(foreach runtime, $(RUNTIMES), $(build-runtime-$(runtime)))
endef

build-platform = $(eval $(call build-platform-impl, $1))


