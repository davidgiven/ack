.define __pause
.extern __pause
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__pause:		move.l #0x1D,d0
		trap #0
		rts
