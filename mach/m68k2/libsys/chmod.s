.define _chmod
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern _chmod
.sect .text
_chmod:		move.w #0xF,d0
		move.l 4(sp),a0
		move.w 8(sp),d1
		ext.l  d1
		jmp callc
