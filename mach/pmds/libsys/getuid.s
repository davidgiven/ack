.define _getuid
.define _geteuid
.extern _getuid
.extern _geteuid
_getuid:		trap #0
.short	0x18
			rts

_geteuid:		trap #0
.short	0x18
			move.l	d1,d0
			rts
