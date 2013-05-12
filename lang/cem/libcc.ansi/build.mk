define build-libcc-ansi-headers-install-one-impl
	$(call reset)
	$(eval q := lang/cem/libcc.ansi/headers/$(strip $1))
	$(call installto, $(PLATIND)/include/ansi/$(strip $1))
	$(eval LIBCCANSIHEADERS += $o)
endef

define build-libcc-ansi-headers-impl
	$(eval g := \
		sys/time.h \
		sys/ioctl.h \
		assert.h \
		ctype.h \
		errno.h \
		float.h \
		limits.h \
		math.h \
		setjmp.h \
		signal.h \
		stdarg.h \
		stddef.h \
		stdint.h \
		stdio.h \
		stdlib.h \
		string.h \
		time.h \
		iso646.h \
		stdbool.h \
		locale.h \
		tgmath.h)

	$(eval LIBCCANSIHEADERS :=)
	$(foreach f,$g,$(call build-libcc-ansi-headers-install-one-impl,$f))
endef

$(eval $(build-libcc-ansi-headers-impl))
