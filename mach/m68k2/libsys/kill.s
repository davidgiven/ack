.define _kill
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern _kill
.sect .text
_kill:		move.w #0x25,d0
		move.w 4(sp),a0
		move.w 6(sp),d1
		ext.l  d1
		jmp callc
