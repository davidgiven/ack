.define _pause
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern _pause
.sect .text
_pause:		move.w #0x1D,d0
		trap #0
		rts
