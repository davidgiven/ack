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


