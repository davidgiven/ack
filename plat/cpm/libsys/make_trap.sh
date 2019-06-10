#!/bin/sh
cat <<EOF
#
#include "asm.h"
.define $1
$1:
    lxi h, $2
	push h
	call .trp
	ret
EOF

