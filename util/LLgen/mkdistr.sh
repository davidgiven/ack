#!/bin/sh
# $Id$
#
# Script to build a distribution package.

set -e
	
if [ "$1" = "" ]; then
	echo "Please specify a version number!"
	exit 1
fi

if [ -d ../LLgen-$1 ]; then
	echo "I think you've already generated version $1."
	exit 1
fi

mkdir ../LLgen-$1
cp -a --parents \
	pm \
	c.pm \
	pmfile \
	doc/LLgen.refs \
	doc/LLgen_NCER.n \
	doc/LLgen.1 \
	doc/LLgen.n \
	lib/rec \
	lib/incl \
	lib/nc_incl \
	lib/nc_rec \
	src/io.h \
	src/extern.h \
	src/reach.c \
	src/LLgen.c \
	src/LLgen.g \
	src/compute.c \
	src/savegram.c \
	src/global.c \
	src/gencode.c \
	src/main.c \
	src/name.c \
	src/types.h \
	src/sets.c \
	src/sets.h \
	src/tokens.c \
	src/tokens.g \
	src/Lpars.c \
	src/Lpars.h \
	src/cclass.c \
	src/cclass.h \
	src/alloc.c \
	src/check.c \
	src/machdep.c \
	mkdistr.sh \
	bootstrap.sh \
	COPYING \
	READ_ME \
\
	../LLgen-$1

(cd .. && tar cvf LLgen-$1.tar.bz2 --bzip2 LLgen-$1)

echo ""
echo "Done --- but did you remember to update the version number in the README?"
