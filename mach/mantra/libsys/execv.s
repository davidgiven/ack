.define _execv
.extern _execv
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_execv:		link a6,#0
		tst.b -48(sp)
		move.l _environ,-(sp)
		move.l 12(a6),-(sp)
		move.l 8(a6),-(sp)
		jsr _execve
		add.l #0xC,sp
		unlk a6
		rts
