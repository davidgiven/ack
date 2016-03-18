ARCH := arm
PCCARCH := arm
PLATFORM := linuxarm
OPTIMISATION := -O6

D := plat/linuxarm/

$(eval $(call build-pcc-platform))

