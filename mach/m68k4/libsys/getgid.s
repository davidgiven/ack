.define _getgid
.extern _getgid
.text
_getgid:	move.w #0x2F,d0
		trap #0
		rts
