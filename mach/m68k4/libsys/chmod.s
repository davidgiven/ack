.define _chmod
.extern _chmod
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_chmod:		move.w #0xF,d0
		move.l 4(sp),a0
		move.l 8(sp),d1
		jmp callc
