.define _access
.extern _access
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_access:	move.l #0x21,d0
		move.l 4(sp),a0
		move.l 8(sp),d1
		jmp callc
