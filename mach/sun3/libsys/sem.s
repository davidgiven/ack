.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _semget,_semctl,_semop
_semget:
	pea	(1)
	bra	1f
_semctl:
	pea	(0)
	bra	1f
_semop:
	pea	(2)
1:
	move.l	(4,sp),d0
	move.l	(sp),(4,sp)
	move.l	d0,(sp)
	jmp	(_semsys)
