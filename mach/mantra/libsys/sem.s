.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _semctl
.define _semop
.define _semget
sem = 53
.extern	_semop
.extern	_semctl
.extern	_semget
.extern	_syscall

_semctl:
 	link	a6,#0
 	tst.b	-32(sp)
 	move.l	20(a6),-(sp)
 	move.l	16(a6),-(sp)
 	move.l	12(a6),-(sp)
 	move.l	8(a6),-(sp)
 	clr.l	-(sp)
 	move.l	#sem,-(sp)
 	jsr	_syscall
 	add	#0x18,sp
 	unlk	a6
 	rts
_semget:
 	link	a6,#0
 	tst.b	-32(sp)
 	move.l	16(a6),-(sp)
 	move.l	12(a6),-(sp)
 	move.l	8(a6),-(sp)
 	move.l	#1,-(sp)
 	move.l	#sem,-(sp)
 	jsr	_syscall
 	add	#0x14,sp
 	unlk	a6
 	rts
_semop:
 	link	a6,#0
 	tst.b	-32(sp)
 	move.l	16(a6),-(sp)
 	move.l	12(a6),-(sp)
 	move.l	8(a6),-(sp)
 	move.l	#2,-(sp)
 	move.l	#sem,-(sp)
 	jsr	_syscall
 	add	#0x14,sp
 	unlk	a6
 	rts
