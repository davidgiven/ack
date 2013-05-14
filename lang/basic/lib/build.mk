define build-runtime-libbasic-impl

$(call reset)
$(eval objdir := $(PLATFORM))

$(call ackfile, lang/basic/lib/fif.e)
$(call ackfile, lang/basic/lib/fef.e)
$(call ackfile, lang/basic/lib/setline.e)
$(call ackfile, lang/basic/lib/abs.c)
$(call ackfile, lang/basic/lib/asc.c)
$(call ackfile, lang/basic/lib/asrt.c)
$(call ackfile, lang/basic/lib/atn.c)
$(call ackfile, lang/basic/lib/chr.c)
$(call ackfile, lang/basic/lib/conversion.c)
$(call ackfile, lang/basic/lib/error.c)
$(call ackfile, lang/basic/lib/exp.c)
$(call ackfile, lang/basic/lib/file.c)
$(call ackfile, lang/basic/lib/hlt.c)
$(call ackfile, lang/basic/lib/io.c)
$(call ackfile, lang/basic/lib/log.c)
$(call ackfile, lang/basic/lib/mki.c)
$(call ackfile, lang/basic/lib/oct.c)
$(call ackfile, lang/basic/lib/peek.c)
$(call ackfile, lang/basic/lib/power.c)
$(call ackfile, lang/basic/lib/print.c)
$(call ackfile, lang/basic/lib/random.c)
$(call ackfile, lang/basic/lib/read.c)
$(call ackfile, lang/basic/lib/return.c)
$(call ackfile, lang/basic/lib/salloc.c)
$(call ackfile, lang/basic/lib/sgn.c)
$(call ackfile, lang/basic/lib/sin.c)
$(call ackfile, lang/basic/lib/sqt.c)
$(call ackfile, lang/basic/lib/stop.c)
$(call ackfile, lang/basic/lib/string.c)
$(call ackfile, lang/basic/lib/swap.c)
$(call ackfile, lang/basic/lib/trace.c)
$(call ackfile, lang/basic/lib/trap.c)
$(call ackfile, lang/basic/lib/write.c)

$(call acklibrary, $(LIBDIR)/$(PLATFORM)/libbasic.a)
$(call installto, $(PLATIND)/$(PLATFORM)/libbasic.a)

endef

build-runtime-libbasic = $(eval $(build-runtime-libbasic-impl))

$(eval RUNTIMES += libbasic)

