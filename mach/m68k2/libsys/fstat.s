.define _fstat
.extern _fstat
_fstat:		move.w	#0x1C,d0
		move.w	4(sp),a0
		move.l	6(sp),d1
		jmp callc
