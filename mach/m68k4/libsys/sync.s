.define _sync
.extern _sync
.text
_sync:		move.w #0x24,d0
		trap #0
		rts
