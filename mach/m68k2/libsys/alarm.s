.define _alarm
.extern _alarm
.text
_alarm:		clr.l d0
		move.w 4(sp),d0
		move.l d0,a0
		move.w #0x1B,d0
		trap #0
		rts
