.define _getegid
.extern _getegid
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_getegid:	move.l #0x2F,d0
		trap #0
		move.l d1,d0
		rts
