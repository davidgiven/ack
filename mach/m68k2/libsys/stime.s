.define _stime
.extern _stime
.text
_stime:		move.w #0x19,d0
		move.l 4(sp),a0
		move.l (a0),a0
		trap #0
		bcs 1f
		rts
1:
		jmp cerror
