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
	pi.h \
	ack/config.h

platform-libsys := \
	_hol0.s \
	errno.s \
	phys_to_user.s \
	user_to_phys.s \
	uart.s \
	creat.c \
	close.c \
	open.c \
	read.c \
	write.c \
	isatty.c \
	brk.c \

ifeq (x,y)
	_sys_rawread.s \
	_sys_rawwrite.s \
	getpid.c \
	kill.c \
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

