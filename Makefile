# ======================================================================= #
#                          ACK CONFIGURATION                              #
#                      (Edit this before building)                        #
# ======================================================================= #

# What platform to build for by default?

DEFAULT_PLATFORM = pc86

# Where should the ACK put its temporary files?

ACK_TEMP_DIR = /tmp

# Where is the ACK going to be installed, eventually? If you don't want to
# install it and just want to run the ACK from the build directory
# (/tmp/ack-build/staging, by default), leave this as $(INSDIR).

#PREFIX = /usr/local
PREFIX = $(INSDIR)

# Where do you want to put the object files used when building?

BUILDDIR = $(ACK_TEMP_DIR)/ack-build

# What build flags do you want to use?

CFLAGS = -g
LDFLAGS = 

# Various commands.

AR = ar
CC = gcc

# Which build system to use; use 'ninja' or 'make' (in lower case). Leave
# blank to autodetect.

BUILDSYSTEM =

# Build flags for ninja.

NINJAFLAGS =

# Build flags for make.

MAKEFLAGS = -r

# ======================================================================= #
#                         END OF CONFIGURATION                            #
# ======================================================================= #

# You shouldn't need to change anything below this point unless you are
# actually developing ACK.

OBJDIR = $(abspath $(BUILDDIR)/obj)
BINDIR = $(abspath $(BUILDDIR)/bin)
LIBDIR = $(abspath $(BUILDDIR)/lib)
INCDIR = $(abspath $(BUILDDIR)/include)
INSDIR = $(abspath $(BUILDDIR)/staging)

PLATIND = $(INSDIR)/share/ack
PLATDEP = $(INSDIR)/lib/ack

MAKECMDGOALS ?= +ack
BUILD_FILES = $(shell find * -name '*.lua')

ifneq ($(shell which ninja),)
BUILDSYSTEM = ninja
BUILDFLAGS = $(NINJAFLAGS)
else
BUILDSYSTEM = make
BUILDFLAGS = $(MAKEFLAGS)
endif

ifneq ($(findstring +, $(MAKECMDGOALS)),)

$(MAKECMDGOALS): $(BUILDDIR)/build.$(BUILDSYSTEM)
	@$(BUILDSYSTEM) $(BUILDFLAGS) -f $^ $(MAKECMDGOALS)

endif

$(BUILDDIR)/build.$(BUILDSYSTEM): first/ackbuilder.lua Makefile $(BUILD_FILES)
	@mkdir -p $(BUILDDIR)
	@lua5.1 first/ackbuilder.lua \
		first/build.lua build.lua \
		--$(BUILDSYSTEM) \
		OBJDIR=$(OBJDIR) \
		BINDIR=$(BINDIR) \
		LIBDIR=$(LIBDIR) \
		INCDIR=$(INCDIR) \
		INSDIR=$(INSDIR) \
		PLATIND=$(PLATIND) \
		PLATDEP=$(PLATDEP) \
		AR=$(AR) \
		CC=$(CC) \
		> $(BUILDDIR)/build.$(BUILDSYSTEM)

clean:
	@rm -rf $(BUILDDIR)

#
#CC = gcc
#AR = ar
#RM = rm -f
#CP = cp
#
#hide = @
#
#CFLAGS += \
#	-I$(INCDIR) \
#	-Imodules/h \
#	-Ih
#
#LDFLAGS +=
#
#ACKFLAGS = -Ih
#
#all: installables
#
#.DELETE_ON_ERROR:
#
#include first/core.mk
#
#include modules/src/object/build.mk
#include modules/src/alloc/build.mk
#include modules/src/input/build.mk
#include modules/src/idf/build.mk
#include modules/src/system/build.mk
#include modules/src/string/build.mk
#include modules/src/print/build.mk
#include modules/src/flt_arith/build.mk
#include modules/src/em_code/build.mk
#include modules/src/em_mes/build.mk
#include modules/src/read_em/build.mk
#
#include util/amisc/build.mk
#include util/cmisc/build.mk
#include util/ack/build.mk
#include util/LLgen/build.mk
#include util/data/build.mk
#include util/opt/build.mk
#include util/ncgg/build.mk
#include util/arch/build.mk
#include util/misc/build.mk
#include util/led/build.mk
#include util/topgen/build.mk
#include util/ego/build.mk
#
#include lang/cem/build.mk
#include lang/basic/build.mk
#include lang/pc/build.mk
#include lang/m2/build.mk
#
#include mach/proto/as/build.mk
#include mach/proto/ncg/build.mk
#include mach/proto/top/build.mk
#
#include plat/linux/liblinux/build.mk
#
#include mach/i80/build.mk
#include mach/i386/build.mk
#include mach/i86/build.mk
#include mach/m68020/build.mk
## include mach/powerpc/build.mk
#include mach/vc4/build.mk
#
#include plat/build.mk
#include plat/pc86/build.mk
#include plat/cpm/build.mk
#include plat/linux386/build.mk
#include plat/linux68k/build.mk
## include plat/linuxppc/build.mk
#include plat/rpi/build.mk
#
#.PHONY: installables
#installables: $(INSTALLABLES)
#	@echo ""
#	@echo "Build completed successfully."
#
#.PHONY: install
#install: installables
#	@echo INSTALLING into $(PREFIX)
#	$(hide) tar cf - -C $(INSDIR) . | tar xvf - -C $(PREFIX)
#
#.PHONY: clean
#clean:
#	@echo CLEAN
#	$(hide) $(RM) $(CLEANABLES)
#
#$(INCDIR)/local.h:
#	@echo LOCAL
#	@mkdir -p $(dir $@)
#	$(hide) echo '#define VERSION 3' > $@
#	$(hide) echo '#define ACKM "$(DEFAULT_PLATFORM)"' >> $@
#	$(hide) echo '#define BIGMACHINE 1' >> $@
#	$(hide) echo '#define SYS_5' >> $@
#
#$(INCDIR)/em_path.h:
#	@echo EM_PATH
#	@mkdir -p $(dir $@)
#	$(hide) echo '#define TMP_DIR "$(ACK_TEMP_DIR)"' > $@
#	$(hide) echo '#define EM_DIR "$(PREFIX)"' >> $@
#	$(hide) echo '#define ACK_PATH "share/ack/descr"' >> $@
#
#-include $(DEPENDS)
