# ======================================================================= #
#                          ACK CONFIGURATION                              #
#                      (Edit this before building)                        #
# ======================================================================= #

# What platform to build for by default?

DEFAULT_PLATFORM ?= pc86

# Where should the ACK put its temporary files?

ACK_TEMP_DIR ?= /tmp

# Where is the ACK going to be installed, eventually? If you don't want to
# install it and just want to run the ACK from the build directory
# (/tmp/ack-build/staging, by default), leave this as $(INSDIR).

ifeq ($(OS),Windows_NT)
PREFIX ?= c:/ack
else
PREFIX ?= /usr/local
#PREFIX = $(INSDIR)
endif

# Where do you want to put the object files used when building?

BUILDDIR ?= $(ACK_TEMP_DIR)/ack-build

# What build flags do you want to use for native code?

CFLAGS ?= -g -Wno-return-type -O3
LDFLAGS ?= 

# Various commands.

AR ?= ar
CC ?= gcc
LUA ?= lua

# Which build system to use; use 'ninja' or 'make' (in lower case). Leave
# blank to autodetect.

BUILDSYSTEM ?=

# Build flags for ninja.

NINJAFLAGS ?=

# Build flags for make.

MAKEFLAGS ?= 

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

.NOTPARALLEL:

ifeq ($(BUILDSYSTEM),)
  ifneq ($(shell which ninja),)
BUILDSYSTEM = ninja
  else
BUILDSYSTEM = make
  endif
endif

build-file = $(BUILDDIR)/build.$(BUILDSYSTEM)
lua-files = $(shell find . -name 'build*.lua')
our-lua = $(BUILDDIR)/lua

# GNU make sets MAKECMDGOALS to the list of targets from the command
# line.  We look for targets with '+' and forward them to BUILDSYSTEM.
# This handles commands like
#     $ make util/opt+pkg util/ego+pkg

all-goals = +ack +tests
plus-goals := $(patsubst all,$(all-goals),$(or $(MAKECMDGOALS),all))
plus-goals := $(foreach g,$(plus-goals),$(if $(findstring +,$(g)),$(g),))

# @true silences extra message, "make: Nothing to be done..."

all: build-plus-goals
	@true

ifneq ($(plus-goals),)
$(plus-goals): build-plus-goals
	@true
endif

build-plus-goals: $(build-file)
ifeq ($(BUILDSYSTEM),ninja)
	@ninja $(NINJAFLAGS) -f $(build-file) $(plus-goals)
else ifeq ($(BUILDSYSTEM),make)
# GNU make passes MAKEFLAGS in environment.
	@$(MAKE) -f $(build-file) $(plus-goals)
else
$(error unknown BUILDSYSTEM = $(BUILDSYSTEM))
endif

$(build-file): first/ackbuilder.lua Makefile $(lua-files)
	@mkdir -p $(BUILDDIR)
	@$(LUA) first/ackbuilder.lua \
		first/build.lua build.lua \
		--$(BUILDSYSTEM) \
		LUA=$(LUA) \
		DEFAULT_PLATFORM=$(DEFAULT_PLATFORM) \
		OBJDIR=$(OBJDIR) \
		BINDIR=$(BINDIR) \
		LIBDIR=$(LIBDIR) \
		INCDIR=$(INCDIR) \
		INSDIR=$(INSDIR) \
		PLATIND=$(PLATIND) \
		PLATDEP=$(PLATDEP) \
		PREFIX="$(PREFIX)" \
		AR=$(AR) \
		CC=$(CC) \
		CFLAGS="$(CFLAGS)" \
		LDFLAGS="$(LDFLAGS)" \
		> $(build-file)

ack-setup.exe: etc/windows-installer.nsi
	makensis -dBUILDDIR=$(BUILDDIR)/staging -dOUTFILE="$$(realpath $@)" $<

install:
	mkdir -p $(PREFIX)
	tar cf - -C $(INSDIR) . | tar xvf - -C $(PREFIX)

clean:
	rm -rf $(BUILDDIR)

