.define _pause
.extern _pause
_pause:			trap #0
.short	0x1D
			rts
