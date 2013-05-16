ARCH := m68020
PLATFORM := linux68k
OPTIMISATION := -O6

D := plat/linux68k/

platform-headers := \
	ack/config.h \
	sys/ioctl.h \
	unistd.h

platform-libsys := \
	_syscall.s

$(eval $(call build-platform))
$(eval $(call build-liblinux))

define build-linux68k-boot-impl
	$(call reset)
	$(call ackfile, $D/boot.s)
	$(call installto, $(PLATIND)/$(PLATFORM)/boot.o)
endef

$(eval $(build-linux68k-boot-impl))

