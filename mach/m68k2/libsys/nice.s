.define _nice
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern _nice
.sect .text
_nice:		move.w #0x22,d0
		move.w 4(sp),a0
		jmp callc
