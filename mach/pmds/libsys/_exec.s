.define __execl
.define __execve
.define __execv
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __execl
.extern __execve
.extern __execv
__execl:		tst.b -48(sp)
		link a6,#0
		move.l _environ,-(sp)
		pea 12(a6)
		move.l 8(a6),-(sp)
		jsr __execve
		add.l #12,sp
		unlk a6
		rts
__execve:	trap #0
.data2 59
		jmp cerror
__execv:		tst.b -48(sp)
		link a6,#0
		move.l _environ,-(sp)
		move.l 12(a6),-(sp)
		move.l 8(a6),-(sp)
		jsr __execve
		add.l #12,sp
		unlk a6
		rts
