ARCH := i86
PLATFORM := pc86
OPTIMISATION := -O

D := plat/pc86/

platform-headers := \
	ack/config.h \
	unistd.h

platform-libsys := \
	errno.s \
	_hol0.s \
	_sys_rawread.s \
	_sys_rawwrite.s \
	open.c \
	creat.c \
	close.c \
	read.c \
	write.c \
	brk.c \
	getpid.c \
	kill.c \
	isatty.c \
	lseek.c \
	time.c \
	signal.c

$(eval $(call build-platform))


