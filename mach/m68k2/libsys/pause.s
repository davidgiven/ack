.define _pause
.extern _pause
.text
_pause:		move.w #0x1D,d0
		trap #0
		rts
