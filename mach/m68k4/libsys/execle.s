.define _execle
.extern _execle
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_execle:	link a6,#0
		tst.b -48(sp)
		lea 12(a6),a0
1:
		tst.l (a0)+
		bne 1b
		move.l a0,-(sp)
		pea 12(a6)
		move.l 8(a6),-(sp)
		jsr _execve
		add.l #0xC,sp
		unlk a6
		rts
