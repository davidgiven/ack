.define _pause
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _pause
_pause:			trap #0
.data2	0x1D
			rts
