ARCH := i80
PLATFORM := cpm
OPTIMISATION := -O

D := plat/cpm

platform-headers := \
	ack/config.h \
	cpm.h \
	unistd.h

platform-libsys := \
	_bdos.s \
	_hol0.s \
	_inn2.s \
	_trap.s \
	brk.c \
	close.c \
	creat.c \
	errno.s \
	getpid.c \
	isatty.c \
	kill.c \
	lseek.c \
	open.c \
	read.c \
	signal.c \
	time.c \
	write.c

$(eval $(call build-platform))



