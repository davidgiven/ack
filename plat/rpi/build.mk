# Build script for Raspberry Pi bare-metal executables (using the
# VideoCore IV processor, not the ARM).
#
# © 2013 David Given
# This file is redistributable under the terms of the 3-clause BSD license.
# See the file 'Copying' in the root of the distribution for the full text.

ARCH := vc4
PLATFORM := rpi
OPTIMISATION := -O

D := plat/rpi/

platform-headers := \
	unistd.h \
	ack/config.h

platform-libsys := \
	_hol0.s \

ifeq (x,y)
	errno.s \
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
endif

$(eval $(call build-platform))

define build-rpi-boot-impl
	$(call reset)
	$(call ackfile, $D/boot.s)
	$(call installto, $(PLATIND)/$(PLATFORM)/boot.o)
endef

$(eval $(build-rpi-boot-impl))

