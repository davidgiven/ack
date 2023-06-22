#
#include "asm.h"

.sect .text
.define .float_unsupported, .fef3, .fif3
.fef3:
.fif3:
.float_unsupported:
	ld hl, message
	jp _panic

message:
	.ascii 'Floating point not supported'
	.data1 0

