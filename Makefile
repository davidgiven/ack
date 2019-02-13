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

PREFIX = /usr/local
#PREFIX = $(INSDIR)

# Where do you want to put the object files used when building?

BUILDDIR = $(ACK_TEMP_DIR)/ack-build

# What build flags do you want to use for native code?

CFLAGS = -g -Wno-return-type
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

MAKEFLAGS = 

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

$(build-file): first/ackbuilder.lua Makefile $(lua-files) $(our-lua)
	@mkdir -p $(BUILDDIR)
	@$(our-lua) first/ackbuilder.lua \
		first/build.lua build.lua \
		--$(BUILDSYSTEM) \
		LUA=$(our-lua) \
		DEFAULT_PLATFORM=$(DEFAULT_PLATFORM) \
		OBJDIR=$(OBJDIR) \
		BINDIR=$(BINDIR) \
		LIBDIR=$(LIBDIR) \
		INCDIR=$(INCDIR) \
		INSDIR=$(INSDIR) \
		PLATIND=$(PLATIND) \
		PLATDEP=$(PLATDEP) \
		PREFIX=$(PREFIX) \
		AR=$(AR) \
		CC=$(CC) \
		CFLAGS="$(CFLAGS)" \
		> $(build-file)

install:
	mkdir -p $(PREFIX)
	tar cf - -C $(INSDIR) . | tar xvf - -C $(PREFIX)

clean:
	rm -rf $(BUILDDIR)

uname := $(shell uname)
ifeq (Linux,$(uname))
# Turn on LUA_USE_POSIX so that Lua is not compiled with the dangerous
# tmpnam(.) function.  But, do not use LUA_USE_LINUX here, since that will
# also turn on LUA_USE_READLINE, and I do not want to force everyone to
# install libreadline-dev.  -- tkchia
$(our-lua): CFLAGS += -DLUA_USE_POSIX -DLUA_USE_DLOPEN
$(our-lua): LDFLAGS += -ldl
else
ifeq (Darwin,$(uname))
$(our-lua): CFLAGS += -DLUA_USE_MACOSX
endif
endif

$(our-lua): first/lua-5.1/*.c first/lua-5.1/*.h
	@echo Bootstrapping build
	@mkdir -p $(BUILDDIR)
	@$(CC) $(CFLAGS) -o $(our-lua) -O first/lua-5.1/*.c $(LDFLAGS) -lm
