.define _sync
.extern _sync
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_sync:		move.l #0x24,d0
		trap #0
		rts
