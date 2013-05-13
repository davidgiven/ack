
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
			$(PLATIND)/descr/$(PLATFORM) \
			$(PLATFORM_HEADERS_$(PLATFORM)) \
			$(ARCHITECTURE_$(ARCH)))

	$(foreach f, $(platform-headers), $(call build-platform-headers, $f))

	$(call reset)
	$(foreach f, $(platform-libsys), $(call ackfile, $D/libsys/$f))
	$(call acklibrary, $(LIBDIR)/$(PLATFORM)/libsys.a)
	$(call installto, $(PLATIND)/$(PLATFORM)/libsys.a)

    $(foreach runtime, $(RUNTIMES), $(build-runtime-$(runtime)))
endef

build-platform = $(eval $(call build-platform-impl, $1))


