.define _getuid
.extern _getuid
.text
_getuid:	move.w #0x18,d0
		trap #0
		rts
