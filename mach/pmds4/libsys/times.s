.define _times
.extern _times
_times:
	trap #0
.short	0x2B
	rts
