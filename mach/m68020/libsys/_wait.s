.define __wait
.extern __wait
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__wait:		move.l	#0x7,d0
		trap #0
		bcs cerror
		tst.l (4,sp)
		beq 1f
		move.l d1,([4,sp])
1:
		rts
