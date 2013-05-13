define build-architecture-impl
	$(eval ARCHITECTURE_$(ARCH) := \
			$(PLATDEP)/$(ARCH)/as \
			$(PLATDEP)/$(ARCH)/ncg)

	$(eval PLATFORM := $(ARCH))
    $(call build-as)
    $(call build-ncg)
endef

build-architecture = $(eval $(build-architecture-impl))

