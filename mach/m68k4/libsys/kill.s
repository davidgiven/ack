.define _kill
.extern _kill
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_kill:		move.w #0x25,d0
		move.l 4(sp),a0
		move.l 6(sp),d1
		jmp callc
