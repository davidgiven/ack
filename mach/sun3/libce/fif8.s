.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define fif8
fif8:
	move.l	(4,sp),d0
	move.l	(8,sp),d1
	lea	(12,sp),a0
	jsr	(Fmuld)		! result in d0/d1
	lea	(4,sp),a0	! build stack for modf
	move.l	a0,-(sp)
	move.l	d1,-(sp)
	move.l	d0,-(sp)
	jsr	(_modf)
	lea	(12,sp),sp
	move.l	d0,(12,sp)
	move.l	d1,(16,sp)
	rts
