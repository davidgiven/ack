ARCH := i386
PLATFORM := linux386
OPTIMISATION := -O

D := plat/linux386/

platform-headers := \
	ack/config.h \
	sys/ioctl.h \
	unistd.h

platform-libsys := \
	_syscall.s

$(eval $(call build-platform))
$(eval $(call build-liblinux))

define build-linux386-boot-impl
	$(call reset)
	$(call ackfile, $D/boot.s)
	$(call installto, $(PLATIND)/$(PLATFORM)/boot.o)
endef

$(eval $(build-linux386-boot-impl))




