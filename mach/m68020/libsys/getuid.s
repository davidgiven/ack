.define _getuid
.extern _getuid
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_getuid:	move.l #0x18,d0
		trap #0
		rts
