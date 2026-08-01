#!/bin/sh

EMULATOR="/usr/local/bin/emu6800"
CPUTYPE="6800"

exec "$EMULATOR" "$CPUTYPE" "$@" /dev/null
