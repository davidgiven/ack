.define _execl
.define _execve
.define _execv
.extern _execl
.extern _execve
.extern _execv
_execl:		tst.b -48(sp)
		link a6,#0
		move.l _environ,-(sp)
		pea 12(a6)
		move.l 8(a6),-(sp)
		jsr _execve
		add.l #12,sp
		unlk a6
		rts
_execve:	trap #0
.short 59
		jmp cerror
_execv:		tst.b -48(sp)
		link a6,#0
		move.l _environ,-(sp)
		move.l 12(a6),-(sp)
		move.l 8(a6),-(sp)
		jsr _execve
		add.l #12,sp
		unlk a6
		rts
