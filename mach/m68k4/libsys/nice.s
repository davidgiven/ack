.define _nice
.extern _nice
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_nice:		move.w #0x22,d0
		move.l 4(sp),a0
		jmp callc
