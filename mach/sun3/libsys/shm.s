.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _shmget,_shmctl,_shmrcv,_shmsnd
_shmget:
	pea	(3)
	bra	1f
_shmctl:
	pea	(1)
	bra	1f
_shmat:
	pea	(4)
	bra	1f
_shmdt:
	pea	(2)
1:
	move.l	(4,sp),d0
	move.l	(sp),(4,sp)
	move.l	d0,(sp)
	jmp	(_shmsys)
