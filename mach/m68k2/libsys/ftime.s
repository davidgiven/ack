.define _ftime
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _ftime
.sect .text
_ftime:		move.w #0x23,d0
		move.l 4(sp),a0
		jmp callc
