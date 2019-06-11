#!/bin/sh
cat <<EOF
#
#include "asm.h"
.define _$2
_$2:
    mvi a, $1
    jmp call_bdos
EOF

