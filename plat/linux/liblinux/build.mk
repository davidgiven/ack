define build-liblinux-impl

$(call reset)
$(eval objdir := $(PLATFORM))
$(call ackfile, plat/linux/liblinux/_exit.c)
$(call ackfile, plat/linux/liblinux/_hol0.s)
$(call ackfile, plat/linux/liblinux/brk.c)
$(call ackfile, plat/linux/liblinux/close.c)
$(call ackfile, plat/linux/liblinux/creat.c)
$(call ackfile, plat/linux/liblinux/errno.s)
$(call ackfile, plat/linux/liblinux/getpid.c)
$(call ackfile, plat/linux/liblinux/gettimeofday.c)
$(call ackfile, plat/linux/liblinux/isatty.c)
$(call ackfile, plat/linux/liblinux/kill.c)
$(call ackfile, plat/linux/liblinux/lseek.c)
$(call ackfile, plat/linux/liblinux/open.c)
$(call ackfile, plat/linux/liblinux/read.c)
$(call ackfile, plat/linux/liblinux/sbrk.c)
$(call ackfile, plat/linux/liblinux/signal.c)
$(call ackfile, plat/linux/liblinux/write.c)

$(call acklibrary, $(OBJDIR)/$D/liblinux.a)
$(call installto, $(PLATIND)/$(PLATFORM)/liblinux.a)

endef

build-liblinux = $(eval $(build-liblinux-impl))
