.define _sync
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _sync
_sync:			trap #0
.data2	0x24
			rts
