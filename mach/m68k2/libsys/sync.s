.define _sync
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern _sync
.sect .text
_sync:		move.w #0x24,d0
		trap #0
		rts
