.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _msgsnd
.define _msgctl
.define _msgrcv
.define _msgget
msg = 49
.extern	_msgrcv
.extern	_msgctl
.extern	_msgsnd
.extern	_msgget
.extern	_syscall

_msgget:
	link	a6,#0
	tst.b	-32(sp)
 	move.l	12(a6),-(sp)
 	move.l	8(a6),-(sp)
 	clr.l	-(sp)
 	move.l	#msg,-(sp)
 	jsr	_syscall
 	add	#0x10,sp
 	unlk	a6
 	rts
_msgctl:
 	link	a6,#0
 	tst.b	-32(sp)
 	move.l	16(a6),-(sp)
 	move.l	12(a6),-(sp)
 	move.l	8(a6),-(sp)
 	move.l	#1,-(sp)
 	move.l	#msg,-(sp)
 	jsr	_syscall
 	add	#0x14,sp
 	unlk	a6
 	rts
_msgrcv:
 	link	a6,#0
 	tst.b	-32(sp)
 	move.l	24(a6),-(sp)
 	move.l	20(a6),-(sp)
 	move.l	16(a6),-(sp)
 	move.l	12(a6),-(sp)
 	move.l	8(a6),-(sp)
 	move.l	#2,-(sp)
 	move.l	#msg,-(sp)
 	jsr	_syscall
 	add	#0x1C,sp
 	unlk	a6
 	rts
_msgsnd:
 	link	a6,#0
 	tst.b	-32(sp)
 	move.l	20(a6),-(sp)
 	move.l	16(a6),-(sp)
 	move.l	12(a6),-(sp)
 	move.l	8(a6),-(sp)
 	move.l	#3,-(sp)
 	move.l	#msg,-(sp)
 	jsr	_syscall
 	add	#0x18,sp
 	unlk	a6
 	rts
