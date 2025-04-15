# ======================================================================= #
#                          ACK CONFIGURATION                              #
#                      (Edit this before building)                        #
# ======================================================================= #

# What platform to build for by default?

DEFAULT_PLATFORM ?= pc86

# Where should the ACK put its temporary files?

ifeq ($(TMPDIR),)
ACK_TEMP_DIR ?= /tmp
else
ACK_TEMP_DIR ?= $(TMPDIR)
endif

INSDIR = $(OBJ)/staging

# Where is the ACK going to be installed, eventually? If you don't want to
# install it and just want to run the ACK from the build directory
# ($(TMPDIR)/ack-build/staging, by default), leave this as $(INSDIR).

ifeq ($(OS),Windows_NT)
PREFIX ?= c:/ack
else
PREFIX ?= /opt/pkg/ack
#PREFIX = $(INSDIR)
endif

# Where do you want to put the object files used when building?

BUILDDIR ?= $(ACK_TEMP_DIR)/ack-build

# What build flags do you want to use for native code?

CFLAGS ?= -g -Os \
	-Werror=return-type \
	-Werror=implicit-function-declaration \
	-Werror=strict-prototypes \
	-DUNREACHABLE_CODE='__builtin_unreachable()' \
	-DNORETURN=_Noreturn

ifeq ($(OS),Windows_NT)
CFLAGS += -DWIN32
endif

HOSTCFLAGS = $(CFLAGS)
ACKCFLAGS = -O

LDFLAGS ?= -g

# Various commands.

LUA ?= lua

.PHONY: all
all: +all

PLATIND = $(INSDIR)/share/ack
PLATDEP = $(INSDIR)/lib/ack

AB_ENABLE_PROGRESS_INFO = false
include build/ab.mk
