# ======================================================================= #
#                          ACK CONFIGURATION                              #
#                      (Edit this before building)                        #
# ======================================================================= #

# What platform to build for by default?

DEFAULT_PLATFORM ?= pc86

# Which architectures should get built?

PLATS = 
# PLATS = all
# PLATS = linux386 linuxppc linuxmips
$(if $(PLATS),, $(error Supply PLATS='something' to specify which architectures to build for. (Use 'all' for all of them.) (Edit the Makefile to set the default.))

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

# Custom rule to build the installer.

ack-setup.exe: etc/windows-installer.nsi all
	makensis -dBUILDDIR="$$(realpath $(INSDIR))" -dOUTFILE="$$(realpath $@)" $<

PLATIND = $(INSDIR)/share/ack
PLATDEP = $(INSDIR)/lib/ack
export PLATS

AB_ENABLE_PROGRESS_INFO = false
include build/ab.mk
