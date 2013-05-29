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
		fcntl.h \
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

define build-runtime-libcc-ansi-impl

$(call reset)
$(eval objdir := $(PLATFORM))

# Assert

$(call ackfile, lang/cem/libcc.ansi/assert/assert.c)

# CType

$(call ackfile, lang/cem/libcc.ansi/ctype/tolower.c)
$(call ackfile, lang/cem/libcc.ansi/ctype/toupper.c)
$(call acktabgen, lang/cem/libcc.ansi/ctype/char.tab)

$(eval g := \
	isalnum.c \
	isalpha.c \
	iscntrl.c \
	isdigit.c \
	isgraph.c \
	islower.c \
	isprint.c \
	ispunct.c \
	isspace.c \
	isupper.c \
	isxdigit.c \
	isascii.c)
$(eval g := $(addprefix $(OBJDIR)/$(objdir)/ctype/, $g))

$(wordlist 2, $(words $g), $g): $(firstword $g)
$(firstword $g): lang/cem/libcc.ansi/ctype/genfiles
	@echo GENFILES $$@
	@mkdir -p $$(dir $$@)
	$(hide) cd $$(dir $$@) && sh $$(abspath $$^)

$(eval CLEANABLES += $g)
$(foreach f, $g, $(call ackfile, $f))

# errno

$(call ackfile, lang/cem/libcc.ansi/errno/errlist.c)

# locale

$(call ackfile, lang/cem/libcc.ansi/locale/localeconv.c)
$(call ackfile, lang/cem/libcc.ansi/locale/setlocale.c)

# math

$(call ackfile, lang/cem/libcc.ansi/math/asin.c)
$(call ackfile, lang/cem/libcc.ansi/math/atan2.c)
$(call ackfile, lang/cem/libcc.ansi/math/atan.c)
$(call ackfile, lang/cem/libcc.ansi/math/ceil.c)
$(call ackfile, lang/cem/libcc.ansi/math/fabs.c)
$(call ackfile, lang/cem/libcc.ansi/math/pow.c)
$(call ackfile, lang/cem/libcc.ansi/math/log10.c)
$(call ackfile, lang/cem/libcc.ansi/math/log.c)
$(call ackfile, lang/cem/libcc.ansi/math/sin.c)
$(call ackfile, lang/cem/libcc.ansi/math/sinh.c)
$(call ackfile, lang/cem/libcc.ansi/math/sqrt.c)
$(call ackfile, lang/cem/libcc.ansi/math/tan.c)
$(call ackfile, lang/cem/libcc.ansi/math/tanh.c)
$(call ackfile, lang/cem/libcc.ansi/math/exp.c)
$(call ackfile, lang/cem/libcc.ansi/math/ldexp.c)
$(call ackfile, lang/cem/libcc.ansi/math/fmod.c)
$(call ackfile, lang/cem/libcc.ansi/math/floor.c)
$(call ackfile, lang/cem/libcc.ansi/math/hugeval.c)
$(call ackfile, lang/cem/libcc.ansi/math/frexp.e)
$(call ackfile, lang/cem/libcc.ansi/math/modf.e)
$(call ackfile, lang/cem/libcc.ansi/math/isnan.c)

# Misc

$(call ackfile, lang/cem/libcc.ansi/misc/environ.c)
# (lots of stuff missing here, as not being appropriate on modern systems)

# setjmp

$(call ackfile, lang/cem/libcc.ansi/setjmp/setjmp.e)

# signal

$(call ackfile, lang/cem/libcc.ansi/signal/raise.c)

# stdio

$(call ackfile, lang/cem/libcc.ansi/stdio/tmpfile.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/tmpnam.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/fopen.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/freopen.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/setbuf.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/setvbuf.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/perror.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/fprintf.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/printf.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/sprintf.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/snprintf.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/vfprintf.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/vprintf.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/vsprintf.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/vsnprintf.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/doprnt.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/icompute.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/fscanf.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/scanf.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/sscanf.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/doscan.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/fgetc.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/fgets.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/getc.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/getchar.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/gets.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/putc.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/putchar.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/fputc.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/puts.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/fputs.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/ungetc.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/fread.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/fwrite.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/fgetpos.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/fsetpos.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/rewind.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/fseek.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/ftell.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/clearerr.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/feof.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/ferror.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/fileno.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/fltpr.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/ecvt.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/fillbuf.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/fclose.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/flushbuf.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/fflush.c)
$(call ackfile, lang/cem/libcc.ansi/stdio/data.c)

# stdlib

$(call ackfile, lang/cem/libcc.ansi/stdlib/abort.c)
$(call ackfile, lang/cem/libcc.ansi/stdlib/abs.c)
$(call ackfile, lang/cem/libcc.ansi/stdlib/atof.c)
$(call ackfile, lang/cem/libcc.ansi/stdlib/atoi.c)
$(call ackfile, lang/cem/libcc.ansi/stdlib/atol.c)
$(call ackfile, lang/cem/libcc.ansi/stdlib/bsearch.c)
$(call ackfile, lang/cem/libcc.ansi/stdlib/div.c)
$(call ackfile, lang/cem/libcc.ansi/stdlib/atexit.c)
$(call ackfile, lang/cem/libcc.ansi/stdlib/exit.c)
$(call ackfile, lang/cem/libcc.ansi/stdlib/getenv.c)
$(call ackfile, lang/cem/libcc.ansi/stdlib/setenv.c)
$(call ackfile, lang/cem/libcc.ansi/stdlib/labs.c)
$(call ackfile, lang/cem/libcc.ansi/stdlib/ldiv.c)
$(call ackfile, lang/cem/libcc.ansi/stdlib/mblen.c)
$(call ackfile, lang/cem/libcc.ansi/stdlib/mbstowcs.c)
$(call ackfile, lang/cem/libcc.ansi/stdlib/mbtowc.c)
$(call ackfile, lang/cem/libcc.ansi/stdlib/qsort.c)
$(call ackfile, lang/cem/libcc.ansi/stdlib/rand.c)
$(call ackfile, lang/cem/libcc.ansi/stdlib/strtod.c)
$(call ackfile, lang/cem/libcc.ansi/stdlib/strtol.c)
$(call ackfile, lang/cem/libcc.ansi/stdlib/wcstombs.c)
$(call ackfile, lang/cem/libcc.ansi/stdlib/wctomb.c)
$(call ackfile, lang/cem/libcc.ansi/stdlib/ext_comp.c)

$(eval g := $(OBJDIR)/$(objdir)/malloc.c)
$(eval CLEANABLES += $g)
$(eval srcs := \
	READ_ME size_type.h param.h impl.h check.h \
	log.h phys.h mal.c log.c phys.c check.c)
$(eval srcs := $(addprefix lang/cem/libcc.ansi/stdlib/malloc/, $(srcs)))

$g: lang/cem/libcc.ansi/stdlib/malloc/add_file $(srcs)
	@echo MALLOC $g
	@mkdir -p $(dir $g)
	$(hide) $(RM) $g && for i in $(srcs); do \
		lang/cem/libcc.ansi/stdlib/malloc/add_file $$$$i >> $g; done

$(call ackfile, $g)

# String

$(call ackfile, lang/cem/libcc.ansi/string/memchr.c)
$(call ackfile, lang/cem/libcc.ansi/string/memcmp.c)
$(call ackfile, lang/cem/libcc.ansi/string/memcpy.c)
$(call ackfile, lang/cem/libcc.ansi/string/memmove.c)
$(call ackfile, lang/cem/libcc.ansi/string/memset.c)
$(call ackfile, lang/cem/libcc.ansi/string/strcat.c)
$(call ackfile, lang/cem/libcc.ansi/string/strchr.c)
$(call ackfile, lang/cem/libcc.ansi/string/strcmp.c)
$(call ackfile, lang/cem/libcc.ansi/string/strcoll.c)
$(call ackfile, lang/cem/libcc.ansi/string/strcpy.c)
$(call ackfile, lang/cem/libcc.ansi/string/strcspn.c)
$(call ackfile, lang/cem/libcc.ansi/string/strerror.c)
$(call ackfile, lang/cem/libcc.ansi/string/strncat.c)
$(call ackfile, lang/cem/libcc.ansi/string/strncpy.c)
$(call ackfile, lang/cem/libcc.ansi/string/strrchr.c)
$(call ackfile, lang/cem/libcc.ansi/string/strstr.c)
$(call ackfile, lang/cem/libcc.ansi/string/strlen.c)
$(call ackfile, lang/cem/libcc.ansi/string/strtok.c)
$(call ackfile, lang/cem/libcc.ansi/string/strpbrk.c)
$(call ackfile, lang/cem/libcc.ansi/string/strspn.c)
$(call ackfile, lang/cem/libcc.ansi/string/strncmp.c)
$(call ackfile, lang/cem/libcc.ansi/string/strxfrm.c)
$(call ackfile, lang/cem/libcc.ansi/string/strdup.c)

# Time

$(call ackfile, lang/cem/libcc.ansi/time/ctime.c)
$(call ackfile, lang/cem/libcc.ansi/time/asctime.c)
$(call ackfile, lang/cem/libcc.ansi/time/localtime.c)
$(call ackfile, lang/cem/libcc.ansi/time/clock.c)
$(call ackfile, lang/cem/libcc.ansi/time/difftime.c)
$(call ackfile, lang/cem/libcc.ansi/time/gmtime.c)
$(call ackfile, lang/cem/libcc.ansi/time/mktime.c)
$(call ackfile, lang/cem/libcc.ansi/time/strftime.c)
$(call ackfile, lang/cem/libcc.ansi/time/time.c)
$(call ackfile, lang/cem/libcc.ansi/time/tzset.c)
$(call ackfile, lang/cem/libcc.ansi/time/misc.c)

$(call acklibrary, $(LIBDIR)/$(PLATFORM)/libc.a)
$(call installto, $(PLATIND)/$(PLATFORM)/libc.a)

# CRT

$(call reset)
$(eval objdir := $(PLATFORM))
$(eval ackflags += -Ih)
$(call ackfile, lang/cem/libcc.ansi/head_ac.e)
$(call installto, $(PLATIND)/$(PLATFORM)/c-ansi.o)

endef

build-runtime-libcc-ansi = $(eval $(build-runtime-libcc-ansi-impl))

$(eval RUNTIMES += libcc-ansi)

