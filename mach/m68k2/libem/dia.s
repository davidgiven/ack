.define	.diagnos
.sect .text
.sect .rom
.sect .data
.sect .bss

space	= 040
del	= 0177

	.sect .text
.diagnos:
	move.w	hol0,-(sp)
	move.l	hol0+4,d2
	beq	1f
	move.l	d2,a0
	move.l	#40,d0
3:
	move.b	(a0)+,d1
	beq	2f
	cmp.b	#del,d1
	bge	1f
	cmp.b	#space,d1
	blt	1f
	sub	#1,d0
	bgt	3b
	clr.b	(a0)
2:
	move.l	d2,-(sp)
	pea	fmt
	jsr	printf
	add	#10,sp
	jmp	printf

1:
	move.l	#unknwn,d2
	bra	2b

.sect .text
putchar:
	move.w	#1,-(sp)
	pea	7(sp)
	move.w	#1,-(sp)
	jsr	_write
	add.l	#8,sp
	rts
printf:
	link	a6,#-12
.sect .data
_12:
	.data2	28786
	.data2	26990
	.data2	29798
	.data2	11875
	.data2	0
.sect .text
	pea	8+4(a6)
	move.l	(sp)+,-6(a6)
I004:
	move.l	8+0(a6),-(sp)
	move.l	(sp),-(sp)
	move.l	(sp)+,a0
	add	#1,a0
	move.l	a0,-(sp)
	move.l	(sp)+,8+0(a6)
	move.l	(sp)+,a0
	clr	d0
	move.b	(a0),d0
	move.w	d0,-(sp)
	move.w	(sp),-(sp)
	move.w	(sp)+,-2(a6)
	move.w	#37,-(sp)
	move.w	(sp)+,d0
	cmp	(sp)+,d0
	beq	I005
	move.w	-2(a6),-(sp)
	tst	(sp)+
	beq	I002
	move.w	-2(a6),-(sp)
	jsr	putchar
	add	#2,sp
	jmp	I004
I005:
	move.l	8+0(a6),-(sp)
	move.l	(sp),-(sp)
	move.l	(sp)+,a0
	add	#1,a0
	move.l	a0,-(sp)
	move.l	(sp)+,8+0(a6)
	move.l	(sp)+,a0
	clr	d0
	move.b	(a0),d0
	move.w	d0,-(sp)
	move.w	(sp)+,-2(a6)
	move.w	-2(a6),-(sp)
	move.w	#100,-(sp)
	move.w	(sp)+,d0
	cmp	(sp)+,d0
	beq	I008
	move.w	-2(a6),-(sp)
	move.w	#117,-(sp)
	move.w	(sp)+,d0
	cmp	(sp)+,d0
	bne	I007
I008:
	move.l	-6(a6),-(sp)
	move.l	(sp)+,a0
	add	#2,a0
	move.l	a0,-(sp)
	move.l	(sp),-(sp)
	move.l	(sp)+,-6(a6)
	move.l	(sp)+,a0
	move.w	-2(a0),-(sp)
	move.w	(sp)+,-8(a6)
	move.w	-2(a6),-(sp)
	move.w	#100,-(sp)
	move.w	(sp)+,d0
	cmp	(sp)+,d0
	bne	I009
	move.w	-8(a6),-(sp)
	tst	(sp)+
	bge	I009
	move.w	#0,-(sp)
	move.w	-8(a6),-(sp)
	move.w	(sp)+,d0
	move.w	(sp)+,d1
	sub	d0,d1
	move.w	d1,-(sp)
	move.w	(sp)+,-8(a6)
	move.w	#45,-(sp)
	jsr	putchar
	add	#2,sp
I009:
	move.w	-8(a6),-(sp)
	jsr	printn
	add	#2,sp
	jmp	I004
I007:
	move.w	-2(a6),-(sp)
	move.w	#115,-(sp)
	move.w	(sp)+,d0
	cmp	(sp)+,d0
	bne	I004
	move.l	-6(a6),-(sp)
	move.l	(sp)+,a0
	add	#4,a0
	move.l	a0,-(sp)
	move.l	(sp),-(sp)
	move.l	(sp)+,-6(a6)
	move.l	(sp)+,a0
	move.l	-4(a0),-(sp)
	move.l	(sp)+,-12(a6)
I00c:
	move.l	-12(a6),-(sp)
	move.l	(sp),-(sp)
	move.l	(sp)+,a0
	add	#1,a0
	move.l	a0,-(sp)
	move.l	(sp)+,-12(a6)
	move.l	(sp)+,a0
	clr	d0
	move.b	(a0),d0
	move.w	d0,-(sp)
	move.w	(sp),-(sp)
	move.w	(sp)+,-2(a6)
	tst	(sp)+
	beq	I004
	move.w	-2(a6),-(sp)
	jsr	putchar
	add	#2,sp
	jmp	I00c
I002:
	unlk	a6
	rts
printn:
	link	a6,#-2
.sect .data
_15:
	.data2	12337
	.data2	12851
	.data2	13365
	.data2	13879
	.data2	14393
	.data2	0
.sect .text
	move.w	8+0(a6),-(sp)
	move.w	#10,-(sp)
	move.w	(sp)+,d0
	clr.l	d1
	move.w	(sp)+,d1
	divu	d0,d1
	move.w	d1,-(sp)
	move.w	(sp),-(sp)
	move.w	(sp)+,-2(a6)
	tst	(sp)+
	beq	I013
	move.w	-2(a6),-(sp)
	jsr	printn
	add	#2,sp
I013:
	pea	_15
	move.w	8+0(a6),-(sp)
	move.w	#10,-(sp)
	move.w	(sp)+,d0
	clr.l	d1
	move.w	(sp)+,d1
	divu	d0,d1
	swap	d1
	move.w	d1,-(sp)
	move.w	(sp)+,d0
	ext.l	d0
	add.l	(sp)+,d0
	move.l	d0,-(sp)
	move.l	(sp)+,a0
	clr	d0
	move.b	(a0),d0
	move.w	d0,-(sp)
	jsr	putchar
	add	#2,sp
	unlk	a6
	rts
	.sect .data
fmt:	.asciz "%s, line %d: "
unknwn:	.asciz "unknown file"
.align 2
