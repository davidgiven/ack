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

