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


