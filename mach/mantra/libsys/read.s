.define _read
.extern _read
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_read:		move.l #0x3,d0
		move.l 4(sp),a0
		move.l 8(sp),d1
		move.l 12(sp),a1
		jmp call
