ARCH := powerpc
PLATFORM := linuxppc
OPTIMISATION := -O6

D := plat/linuxppc/

platform-headers := \
	ack/config.h \
	sys/ioctl.h \
	unistd.h

platform-libsys := \
	_syscall.s \
	trap.s

$(eval $(call build-platform))
$(eval $(call build-liblinux))

define build-linuxppc-boot-impl
	$(call reset)
	$(call ackfile, $D/boot.s)
	$(call installto, $(PLATIND)/$(PLATFORM)/boot.o)
endef

$(eval $(build-linuxppc-boot-impl))






