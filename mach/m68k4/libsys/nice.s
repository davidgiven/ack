.define _nice
.extern _nice
.text
_nice:		move.w #0x22,d0
		move.w 4(sp),a0
		jmp callc
