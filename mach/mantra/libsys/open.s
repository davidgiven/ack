.define _open
.extern _open
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_open:		move.l #0x5,d0
		move.l 4(sp),a0
		move.l 8(sp),d1
		jmp call
