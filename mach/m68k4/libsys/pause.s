.define _pause
.extern _pause
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_pause:		move.w #0x1D,d0
		trap #0
		rts
