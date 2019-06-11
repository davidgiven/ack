#!/bin/sh
cat <<EOF
#
#include "asm.h"
.define $1
$1:
	mvi a, $2
	jmp .trp
EOF

