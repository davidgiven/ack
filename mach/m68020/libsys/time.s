.define _time
.extern _time
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_time:		move.l #0xD,d0
		trap #0
		tst.l (4,sp)
		beq 1f
		move.l d0,([4,sp])
1:
		rts
