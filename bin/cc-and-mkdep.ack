#!/bin/sh
: '$Id$'

: Compile and make dependencies. First argument is the file on which the
: dependencies must be produced. This version is for ACK.
n=$1
shift
exec $CC -Rcem-A$n -Rcem-m $*
