.define _getuid
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern _getuid
.sect .text
_getuid:	move.w #0x18,d0
		trap #0
		rts
