MAKENOT4 := $(if $(findstring 3.9999, $(lastword $(sort 3.9999 $(MAKE_VERSION)))),yes,no)

ifeq ($(MAKENOT4),yes)
$(error You need GNU Make 4.x for this (if you're on OSX, use gmake).)
endif

OBJ ?= .obj
PYTHON ?= python3
PKG_CONFIG ?= pkg-config
HOST_PKG_CONFIG ?= $(PKG_CONFIG)
ECHO ?= echo
CP ?= cp

HOSTCC ?= gcc
HOSTCXX ?= g++
HOSTAR ?= ar
HOSTCFLAGS ?= -g -Og
HOSTCXXFLAGS ?= $(HOSTCFLAGS)
HOSTLDFLAGS ?= -g

CC ?= $(HOSTCC)
CXX ?= $(HOSTCXX)
AR ?= $(HOSTAR)
CFLAGS ?= $(HOSTCFLAGS)
CXXFLAGS ?= $(CFLAGS)
LDFLAGS ?= $(HOSTLDFLAGS)

NINJA ?= ninja

ifdef VERBOSE
	hide =
else
	ifdef V
		hide =
	else
		hide = @
	endif
endif

# If enabled, shows a nice display of how far through the build you are. This
# doubles Make startup time. Also, on Make 4.3 and above, rebuilds don't show
# correct progress information.
AB_ENABLE_PROGRESS_INFO ?= true

WINDOWS := no
OSX := no
LINUX := no
ifeq ($(OS),Windows_NT)
    WINDOWS := yes
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
		LINUX := yes
    endif
    ifeq ($(UNAME_S),Darwin)
		OSX := yes
    endif
endif

ifeq ($(OS), Windows_NT)
	EXT ?= .exe
endif
EXT ?=

CWD=$(shell pwd)

define newline


endef

define check_for_command
  $(shell command -v $1 >/dev/null || (echo "Required command '$1' missing" >/dev/stderr && kill $$PPID))
endef

$(call check_for_command,ninja)
$(call check_for_command,cmp)
$(call check_for_command,$(PYTHON))

pkg-config-hash = $(shell ($(PKG_CONFIG) --list-all && $(HOST_PKG_CONFIG) --list-all) | md5sum)
build-files = $(shell find . -name .obj -prune -o \( -name 'build.py' -a -type f \) -print) $(wildcard build/*.py) $(wildcard config.py)
build-file-timestamps = $(shell ls -l $(build-files) | md5sum)

# Wipe the build file (forcing a regeneration) if the make environment is different.
# (Conveniently, this includes the pkg-config hash calculated above.)

ignored-variables = MAKE_RESTARTS .VARIABLES MAKECMDGOALS MAKEFLAGS MFLAGS
$(shell mkdir -p $(OBJ))
$(file >$(OBJ)/newvars.txt,$(foreach v,$(filter-out $(ignored-variables),$(.VARIABLES)),$(v)=$($(v))$(newline)))
$(shell touch $(OBJ)/vars.txt)
#$(shell diff -u $(OBJ)/vars.txt $(OBJ)/newvars.txt > /dev/stderr)
$(shell cmp -s $(OBJ)/newvars.txt $(OBJ)/vars.txt || (rm -f $(OBJ)/build.ninja && echo "Environment changed --- regenerating" > /dev/stderr))
$(shell mv $(OBJ)/newvars.txt $(OBJ)/vars.txt)

.PHONY: update-ab
update-ab:
	@echo "Press RETURN to update ab from the repository, or CTRL+C to cancel." \
		&& read a \
		&& (curl -L https://github.com/davidgiven/ab/releases/download/dev/distribution.tar.xz | tar xvJf -) \
		&& echo "Done."

.PHONY: clean
clean::
	@echo CLEAN
	$(hide) rm -rf $(OBJ)

export PYTHONHASHSEED = 1
$(OBJ)/build.ninja $(OBJ)/build.targets &:
	@echo "AB"
	$(hide) $(PYTHON) -X pycache_prefix=$(OBJ)/__pycache__ build/ab.py \
		-o $(OBJ) build.py \
		-v $(OBJ)/vars.txt \
		|| (rm -f $@ && false)

include $(OBJ)/build.targets
.PHONY: $(ninja-targets)
.NOTPARALLEL:
$(ninja-targets): $(OBJ)/build.ninja
	+$(hide) $(NINJA) -f $(OBJ)/build.ninja $@
