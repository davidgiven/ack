.define _sync
.extern _sync
_sync:			trap #0
.short	0x24
			rts
