BUILDDIR = /tmp/obj
OBJDIR = $(BUILDDIR)/obj
BINDIR = $(BUILDDIR)/bin
LIBDIR = $(BUILDDIR)/lib
INCDIR = $(BUILDDIR)/include
INSDIR = $(BUILDDIR)/staging

PLATIND = $(INSDIR)/share/ack
PLATDEP = $(INSDIR)/lib/ack

CC = gcc
AR = ar
RM = rm -f
CP = cp

hide = @

CFLAGS = \
	-g \
	-I$(INCDIR) \
	-Imodules/h \
	-Ih

LDFLAGS =

all: installables

.DELETE_ON_ERROR:

include first/core.mk

include modules/src/object/build.mk
include modules/src/alloc/build.mk
include modules/src/input/build.mk
include modules/src/idf/build.mk
include modules/src/system/build.mk
include modules/src/string/build.mk
include modules/src/print/build.mk
include modules/src/flt_arith/build.mk
include modules/src/em_code/build.mk
include modules/src/em_mes/build.mk
include modules/src/read_em/build.mk

include util/amisc/build.mk
include util/cmisc/build.mk
include util/ack/build.mk
include util/LLgen/build.mk
include util/data/build.mk
include util/opt/build.mk
include util/ncgg/build.mk
include util/arch/build.mk
include util/misc/build.mk

include lang/cem/build.mk

include mach/proto/as/build.mk
include mach/proto/ncg/build.mk

include plat/linux/liblinux/build.mk

include plat/build.mk
include plat/pc86/build.mk
include plat/cpm/build.mk
include plat/linux386/build.mk
include plat/linux68k/build.mk
include plat/linuxppc/build.mk

.PHONY: installables
installables: $(INSTALLABLES)

.PHONY: clean
clean:
	@echo CLEAN
	$(hide) $(RM) $(CLEANABLES)

$(INCDIR)/local.h:
	@echo LOCAL
	@mkdir -p $(dir $@)
	$(hide) echo '#define VERSION 3' > $@
	$(hide) echo '#define ACKM "pc86"' >> $@
	$(hide) echo '#define BIGMACHINE 1' >> $@
	$(hide) echo '#define SYS_5' >> $@

$(INCDIR)/em_path.h:
	@echo EM_PATH
	@mkdir -p $(dir $@)
	$(hide) echo '#define TMP_DIR "/tmp"' > $@
	$(hide) echo '#define EM_DIR "/tmp/obj/staging"' >> $@
	$(hide) echo '#define ACK_PATH "share/ack/descr"' >> $@

-include $(DEPENDS)
