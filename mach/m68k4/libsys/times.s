.define _times
.extern _times
.text
_times:		move.w #0x2B,d0
		move.l 4(sp),a0
		trap #0
		rts
