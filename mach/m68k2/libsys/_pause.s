.define __pause
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern __pause
.sect .text
__pause:		move.w #0x1D,d0
		trap #0
		rts
