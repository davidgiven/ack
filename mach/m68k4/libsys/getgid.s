.define _getgid
.extern _getgid
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_getgid:	move.w #0x2F,d0
		trap #0
		rts
