.define _fstat
.extern _fstat
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_fstat:		move.w	#0x1C,d0
		move.l	4(sp),a0
		move.l	8(sp),d1
		jmp callc
