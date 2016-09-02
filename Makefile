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

install:
	mkdir -p $(PREFIX)
	tar cf - -C $(INSDIR) . | tar xvf - -C $(PREFIX)

clean:
	@rm -rf $(BUILDDIR)

