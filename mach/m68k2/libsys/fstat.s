.define _fstat
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern _fstat
		.sect .text
_fstat:		move.w	#0x1C,d0
		move.w	4(sp),a0
		move.l	6(sp),d1
		jmp callc
