.define _execl
.extern _execl
_execl:		link a6,#0
		tst.b -132(sp)
		move.l _environ,-(sp)
		pea 12(sp)
		move.l 8(a6),-(sp)
		jsr _execve
		add.l #0xC,sp
		unlk a6
		rts
