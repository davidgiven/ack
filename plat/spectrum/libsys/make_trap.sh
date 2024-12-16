#!/bin/sh
cat <<EOF
#
#include "asm.h"
.define $1
$1:
	ld a, $2
	jp .trp
EOF

