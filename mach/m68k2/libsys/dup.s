.define _dup
.extern _dup
.text
_dup:		move.w #0x29,d0
		move.w 4(sp),a0
		jmp call
