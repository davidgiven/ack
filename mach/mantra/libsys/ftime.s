.define _ftime
.extern _ftime
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_ftime:		move.l #0x23,d0
		move.l 4(sp),a0
		jmp callc
