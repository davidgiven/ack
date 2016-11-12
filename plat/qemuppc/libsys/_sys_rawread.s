#
#include "powerpc.h"

! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

! Reads a single byte.

.define __sys_rawread
__sys_rawread:
	li32 r3, 0
	bclr ALWAYS, 0, 0
	