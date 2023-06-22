#
#include "asm.h"

.sect .text
.define .float_unsupported, .fef8, .fif8
.fef8:
.fif8:
.float_unsupported:
	ld hl, message
	jp _panic

message:
	.ascii 'Floating point not supported'
	.data1 0

