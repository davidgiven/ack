.define _execl
.define _execve
.extern _execl
.extern _execve
_execl:		tst.b -48(sp)
		link a6,#0
		move.l _environ,-(sp)
		pea 12(a6)
		move.l 8(a6),-(sp)
		jsr _execve
_execve:	trap #0
.short 59
		jmp cerror
