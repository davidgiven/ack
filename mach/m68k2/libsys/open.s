.define _open
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern _open
.sect .text
_open:		move.w #0x5,d0
		move.l 4(sp),a0
		move.w 8(sp),d1
		ext.l  d1
		jmp call
