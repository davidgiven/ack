#
#include "powerpc.h"

! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

! Writes a single byte to the console.

.define __sys_rawwrite
.extern __sys_rawwrite

__sys_rawwrite:
	bclr ALWAYS, 0, 0
	