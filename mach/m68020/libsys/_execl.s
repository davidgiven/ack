.define __execl
.extern __execl
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__execl:		link a6,#0
		move.l (_environ),-(sp)
		pea (12,a6)
		move.l (8,a6),-(sp)
		jsr (_execve)
		add.l #0xC,sp
		unlk a6
		rts
