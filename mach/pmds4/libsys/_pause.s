.define __pause
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __pause
__pause:			trap #0
.data2	0x1D
			rts
