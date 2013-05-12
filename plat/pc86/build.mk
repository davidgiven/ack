ARCH := i86
PLATFORM := pc86
OPTIMISATION := -O

D := plat/pc86/

$(eval $(call build-platform, \
	ack/config.h \
	unistd.h \
))

include plat/pc86/libsys/build.mk


