.define _execl
.define _execve
.define _execv
.define _execle
.extern _execl
.extern _execve
.extern _execv
.extern _execle
_execl:		tst.b -48(sp)
		link a6,#0
		move.l _environ,-(sp)
pusha:
		pea 12(a6)
pushp:
		move.l 8(a6),-(sp)
		jsr _execve
		unlk	a6
		rts
_execve:	trap #0
.short 3B
		jmp cerror
_execv:		
		tst.b -48(sp)
		link a6,#0
		move.l _environ,-(sp)
		move.l	12(a6),-(sp)
		br	pushp
_execle:	tst.b	-48(sp)
		link	a6,#0
		lea	12(a6),a0
1:
		tst.l	(a0)+
		bne	1b
		move.l	a0,-(sp)
		br	pusha
