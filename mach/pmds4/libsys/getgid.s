.define _getgid
.define _getegid
.extern _getgid
.extern _getegid
_getgid:		trap #0
.short	0x2F
			rts

_getegid:		trap #0
.short	0x2F
			move.l	d1,d0
			rts
