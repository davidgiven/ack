.define _getpid
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern _getpid
.sect .text
_getpid:	move.w #0x14,d0
		trap #0
		rts
