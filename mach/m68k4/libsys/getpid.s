.define _getpid
.extern _getpid
.text
_getpid:	move.w #0x14,d0
		trap #0
		rts
