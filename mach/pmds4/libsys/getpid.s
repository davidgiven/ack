.define _getpid
.extern _getpid
_getpid:		trap #0
.short	0x14
			rts
