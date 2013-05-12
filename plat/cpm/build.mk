ARCH := i80
PLATFORM := cpm
OPTIMISATION := -O

D := plat/cpm

$(eval $(call build-platform, \
	ack/config.h \
	cpm.h \
	unistd.h \
))

include plat/cpm/libsys/build.mk

