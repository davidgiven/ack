.define _getgid
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern _getgid
.sect .text
_getgid:	move.w #0x2F,d0
		trap #0
		rts
