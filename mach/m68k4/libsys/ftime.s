.define _ftime
.extern _ftime
.text
_ftime:		move.w #0x23,d0
		move.l 4(sp),a0
		jmp callc
