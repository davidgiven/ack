.define __execl
.define __execve
.define __execv
.define __execle
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__execl:		tst.b -48(sp)
		link a6,#0
		move.l _environ,-(sp)
3:
		pea 12(a6)
4:
		move.l 8(a6),-(sp)
		jsr __execve
		unlk	a6
		rts
__execve:	trap #0
.data2 0x3b
		jmp cerror
__execv:		
		tst.b -48(sp)
		link a6,#0
		move.l _environ,-(sp)
		move.l	12(a6),-(sp)
		bra	4b
__execle:	tst.b	-48(sp)
		link	a6,#0
		lea	12(a6),a0
1:
		tst.l	(a0)+
		bne	1b
		move.l	a0,-(sp)
		bra	3b
