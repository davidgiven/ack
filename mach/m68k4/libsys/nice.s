.define _nice
.extern _nice
.text
_nice:		move.w #0x22,d0
		move.l 4(sp),a0
		jmp callc
