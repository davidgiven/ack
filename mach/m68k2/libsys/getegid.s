.define _getegid
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern _getegid
.sect .text
_getegid:	move.w #0x2F,d0
		trap #0
		move.l d1,d0
		rts
