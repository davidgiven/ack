.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _shmdt
.define _shmat
.define _shmctl
.define _shmget
shm = 52
.extern	_shmdt
.extern	_shmat
.extern	_shmctl
.extern	_shmget
.extern	_syscall

_shmat:
 	link	a6,#0
 	tst.b	-32(sp)
 	move.l	16(a6),-(sp)
 	move.l	12(a6),-(sp)
 	move.l	8(a6),-(sp)
 	clr.l	-(sp)
 	move.l	#shm,-(sp)
 	jsr	_syscall
 	add	#0x14,sp
 	unlk	a6
 	rts
_shmctl:
 	link	a6,#0
 	tst.b	-32(sp)
 	move.l	16(a6),-(sp)
 	move.l	12(a6),-(sp)
 	move.l	8(a6),-(sp)
 	move.l	#1,-(sp)
 	move.l	#shm,-(sp)
 	jsr	_syscall
 	add	#0x14,sp
 	unlk	a6
 	rts
_shmdt:
 	link	a6,#0
 	tst.b	-144(sp)
 	move.l	8(a6),-(sp)
 	move.l	#2,-(sp)
 	move.l	#shm,-(sp)
 	jsr	_syscall
 	add	#0xC,sp
 	unlk	a6
 	rts
_shmget:
 	link	a6,#0
 	tst.b	-32(sp)
 	move.l	16(a6),-(sp)
 	move.l	12(a6),-(sp)
 	move.l	8(a6),-(sp)
 	move.l	#3,-(sp)
 	move.l	#shm,-(sp)
 	jsr	_syscall
 	add	#0x14,sp
 	unlk	a6
 	rts
