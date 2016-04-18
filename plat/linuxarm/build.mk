ARCH := arm
PCCARCH := arm
PLATFORM := linuxarm
OPTIMISATION := -O6

D := plat/linuxarm/

platform-headers := \
	ack/config.h \
	unistd.h

libc-ansi-linuxarm-wants-math := n

$(eval $(call build-pcc-platform))

define build-linuxarm-boot-impl
	$(call reset)
	$(call ackfile, $D/boot.s)
	$(call installto, $(PLATIND)/$(PLATFORM)/boot.o)
endef

$(eval $(build-linuxarm-boot-impl))

