#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _sbrk,_brk
_sbrk:
	move.l	4(sp),d0
	add.l	#3,d0
	move.l	#~3,d1
	and.l	d1,d0
	move.l	d0,a0
	move.l	.limhp,d0
	add.l	#3,d0
	and.l	d1,d0
	move.l	d0,.limhp
	add.l	d0,a0
	move.l	a0,4(sp)
	pea	17	! not SYS_sbrk
	trap	#0
	bcs	1f
	move.l	.limhp,d0
	move.l	4(sp),.limhp
	rts
1:
	jmp	cerror

_brk:
	move.l	4(sp),d0
	add.l	#3,d0
	move.l	#~3,d1
	and.l	d1,d0
	move.l	d0,4(sp)
	pea	17
	trap	#0
	bcs	1b
	move.l	4(sp),.limhp
	clr.l	d0
	rts
