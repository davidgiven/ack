.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _msgget,_msgctl,_msgrcv,_msgsnd
_msgget:
	pea	(0)
	bra	1f
_msgctl:
	pea	(1)
	bra	1f
_msgrcv:
	pea	(2)
	bra	1f
_msgsnd:
	pea	(3)
1:
	move.l	(4,sp),d0
	move.l	(sp),(4,sp)
	move.l	d0,(sp)
	jmp	(_msgsys)
