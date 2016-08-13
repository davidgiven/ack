define build-runtime-libmodula2-impl

$(call reset)
$(eval objdir := $(PLATFORM))

$(call ackfile, lang/m2/libm2/Arguments.c)
$(call ackfile, lang/m2/libm2/ArraySort.mod)
$(call ackfile, lang/m2/libm2/CSP.mod)
$(call ackfile, lang/m2/libm2/Conversion.mod)
$(call ackfile, lang/m2/libm2/EM.e)
$(call ackfile, lang/m2/libm2/InOut.mod)
$(call ackfile, lang/m2/libm2/LtoUset.e)
$(call ackfile, lang/m2/libm2/MathLib0.mod)
$(call ackfile, lang/m2/libm2/Mathlib.mod)
$(call ackfile, lang/m2/libm2/PascalIO.mod)
$(call ackfile, lang/m2/libm2/Processes.mod)
$(call ackfile, lang/m2/libm2/RealConver.mod)
$(call ackfile, lang/m2/libm2/RealInOut.mod)
$(call ackfile, lang/m2/libm2/SYSTEM.c)
$(call ackfile, lang/m2/libm2/Semaphores.mod)
$(call ackfile, lang/m2/libm2/Storage.mod)
$(call ackfile, lang/m2/libm2/StrAss.c)
$(call ackfile, lang/m2/libm2/Streams.mod)
$(call ackfile, lang/m2/libm2/Strings.mod)
$(call ackfile, lang/m2/libm2/Termcap.mod)
$(call ackfile, lang/m2/libm2/Traps.mod)
$(call ackfile, lang/m2/libm2/XXTermcap.c)
$(call ackfile, lang/m2/libm2/absd.c)
$(call ackfile, lang/m2/libm2/absf.e)
$(call ackfile, lang/m2/libm2/absi.c)
$(call ackfile, lang/m2/libm2/absl.c)
$(call ackfile, lang/m2/libm2/blockmove.c)
$(call ackfile, lang/m2/libm2/cap.c)
$(call ackfile, lang/m2/libm2/catch.c)
$(call ackfile, lang/m2/libm2/confarray.c)
$(call ackfile, lang/m2/libm2/dvi.c)
$(call ackfile, lang/m2/libm2/halt.c)
$(call ackfile, lang/m2/libm2/init.c)
$(call ackfile, lang/m2/libm2/load.c)
$(call ackfile, lang/m2/libm2/par_misc.e)
$(call ackfile, lang/m2/libm2/random.mod)
$(call ackfile, lang/m2/libm2/rcka.c)
$(call ackfile, lang/m2/libm2/rcki.c)
$(call ackfile, lang/m2/libm2/rckil.c)
$(call ackfile, lang/m2/libm2/rcku.c)
$(call ackfile, lang/m2/libm2/rckul.c)
$(call ackfile, lang/m2/libm2/sigtrp.c)
$(call ackfile, lang/m2/libm2/stackprio.c)
$(call ackfile, lang/m2/libm2/store.c)
$(call ackfile, lang/m2/libm2/ucheck.c)

$(eval $q: $(EM_M2))

$(call acklibrary, $(LIBDIR)/$(PLATFORM)/libmodula2.a)
$(call installto, $(PLATIND)/$(PLATFORM)/libmodula2.a)

$(call reset)
$(eval objdir := $(PLATFORM))
$(call ackfile, lang/m2/libm2/head_m2.e)
$(call installto, $(PLATIND)/$(PLATFORM)/modula2.o)

endef

build-runtime-libmodula2 = $(eval $(build-runtime-libmodula2-impl))

$(eval RUNTIMES += libmodula2)

define build-headers-libmodula2-impl

$(eval g := \
	Arguments.def \
	ArraySort.def \
	ASCII.def \
	Conversion.def \
	CSP.def \
	EM.def \
	Epilogue.def \
	InOut.def \
	MathLib0.def \
	Mathlib.def \
	PascalIO.def \
	Processes.def \
	random.def \
	RealConver.def \
	RealInOut.def \
	Semaphores.def \
	Storage.def \
	Streams.def \
	Strings.def \
	StripUnix.def \
	Termcap.def \
	Traps.def \
	Unix.def \
	XXTermcap.def)

$(foreach f, $g, \
	$(call reset) \
	$(call rawfile, lang/m2/libm2/$f) \
	$(call installto, $(PLATIND)/include/modula2/$f))

endef

$(eval $(build-headers-libmodula2-impl))
