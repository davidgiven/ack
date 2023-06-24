#!/bin/sh
cat <<EOF
#
#include "asm.h"

.sect .text
.define .trap_$1
.trap_$1:
	ld a, $2
	jp .trp
EOF

