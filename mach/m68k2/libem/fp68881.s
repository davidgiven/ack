.define .adf4, .adf8, .sbf4, .sbf8, .mlf4, .mlf8, .dvf4, .dvf8
.define .ngf4, .ngf8, .fif4, .fif8, .fef4, .fef8
.define .cif4, .cif8, .cuf4, .cuf8, .cfi, .cfu, .cff4, .cff8
.define .cmf4, .cmf8
.sect .text
.sect .rom
.sect .data
.sect .bss

!	$Header$

!	Implement interface to floating point package for M68881

	.sect .text
.adf4:
	move.l	(sp)+,a0
	fmove.s (sp),fp0
	fadd.s	4(sp),fp0
	fmove.s	fp0,4(sp)
	jmp	(a0)

.adf8:
	move.l	(sp)+,a0
	fmove.d (sp),fp0
	fadd.d	8(sp),fp0
	fmove.d	fp0,8(sp)
	jmp	(a0)

.sbf4:
	move.l	(sp)+,a0
	fmove.s (sp),fp0
	fmove.s	4(sp),fp1
	fsub	fp0,fp1
	fmove.s	fp1,4(sp)
	jmp	(a0)

.sbf8:
	move.l	(sp)+,a0
	fmove.d (sp),fp0
	fmove.d	8(sp),fp1
	fsub	fp0,fp1
	fmove.d	fp1,8(sp)
	jmp	(a0)

.mlf4:
	move.l	(sp)+,a0
	fmove.s (sp),fp0
	fmul.s	4(sp),fp0
	fmove.s	fp0,4(sp)
	jmp	(a0)

.mlf8:
	move.l	(sp)+,a0
	fmove.d (sp),fp0
	fmul.d	8(sp),fp0
	fmove.d	fp0,8(sp)
	jmp	(a0)

.dvf4:
	move.l	(sp)+,a0
	fmove.s (sp),fp0
	fmove.s	4(sp),fp1
	fdiv	fp0,fp1
	fmove.s	fp1,4(sp)
	jmp	(a0)

.dvf8:
	move.l	(sp)+,a0
	fmove.d (sp),fp0
	fmove.d	8(sp),fp1
	fdiv	fp0,fp1
	fmove.d	fp1,8(sp)
	jmp	(a0)

.ngf4:
	fmove.s	4(sp),fp0
	fneg	fp0
	fmove.s	fp0,4(sp)
	rts

.ngf8:
	fmove.d	4(sp),fp0
	fneg	fp0
	fmove.d	fp0,4(sp)
	rts

.fif4:
	move.l	(sp)+,a0
	move.l	(sp),a1
	fmove.s	4(sp),fp0
	fmove.s	8(sp),fp1
	fmul	fp0,fp1
	fintrz	fp1,fp0
	fsub	fp0,fp1
	fmove.s	fp1,4(a1)
	fmove.s	fp0,(a1)
	jmp	(a0)

.fif8:
	move.l	(sp)+,a0
	move.l	(sp),a1
	fmove.d	4(sp),fp0
	fmove.d	12(sp),fp1
	fmul	fp0,fp1
	fintrz	fp1,fp0
	fsub	fp0,fp1
	fmove.d	fp1,8(a1)
	fmove.d	fp0,(a1)
	jmp	(a0)

.fef4:
	move.l	(sp)+,a0
	move.l	(sp),a1
	fmove.s	4(sp),fp0
	fgetexp	fp0,fp1
	fmove.l	fpsr,d0
	and.l	#0x2000,d0	! set if Infinity
	beq	1f
	move.w	#129,(a1)
	fmove.s	4(sp),fp0
	fblt	2f
	move.l	#0x3f000000,2(a1)
	jmp	(a0)
2:
	move.l	#0xbf000000,2(a1)
	jmp	(a0)
1:
	fmove.l	fp1,d0
	add.l	#1,d0
	fgetman	fp0
	fbne	1f
	clr.l	d0
	bra	2f
1:
	fmove.l	#2,fp1
	fdiv	fp1,fp0
2:
	fmove.s	fp0,2(a1)
	move.w	d0,(a1)
	jmp	(a0)

.fef8:
	move.l	(sp)+,a0
	move.l	(sp),a1
	fmove.d	4(sp),fp0
	fgetexp	fp0,fp1
	fmove.l	fpsr,d0
	and.l	#0x2000,d0	! set if Infinity
	beq	1f
	move.w	#1025,(a1)
	fmove.d	4(sp),fp0
	fblt	2f
	move.l	#0x3fe00000,2(a1)
	clr.l	6(a1)
	jmp	(a0)
2:
	move.l	#0xbfe00000,2(a1)
	clr.l	6(a1)
	jmp	(a0)
1:
	fmove.l	fp1,d0
	add.l	#1,d0
	fgetman	fp0
	fbne	1f
	clr.l	d0
	bra	2f
1:
	fmove.l	#2,fp1
	fdiv	fp1,fp0
2:
	fmove.d	fp0,2(a1)
	move.w	d0,(a1)
	jmp	(a0)

.cif4:
	move.l	(sp)+,a0
	cmp.w	#2,(sp)
	bne	1f
	fmove.w	2(sp),fp0
	fmove.s	fp0,(sp)
	jmp	(a0)
1:
	fmove.l	2(sp),fp0
	fmove.s	fp0,2(sp)
	jmp	(a0)

.cif8:
	move.l	(sp)+,a0
	cmp.w	#2,(sp)
	bne	1f
	fmove.w	2(sp),fp0
	fmove.d	fp0,(sp)
	jmp	(a0)
1:
	fmove.l	2(sp),fp0
	fmove.d	fp0,(sp)
	jmp	(a0)

.cuf4:
	move.l	(sp)+,a0
	cmp.w	#2,(sp)
	bne	2f
	fmove.w	2(sp),fp0
	tst.w	2(sp)
	bge	1f
	fadd.l	#65536,fp0
1:
	fmove.s	fp0,(sp)
	jmp	(a0)
2:
	fmove.l	2(sp),fp0
	tst.l	2(sp)
	bge	1f
	fsub.l	#-2147483648,fp0
	fsub.l	#-2147483648,fp0
1:
	fmove.s	fp0,2(sp)
	jmp	(a0)

.cuf8:
	move.l	(sp)+,a0
	move.w	(sp),d0
	cmp.w	#2,d0
	bne	2f
	fmove.w	2(sp),fp0
	tst.w	2(sp)
	bge	1f
	fadd.l	#65536,fp0
	bra	1f
2:
	fmove.l	2(sp),fp0
	tst.l	2(sp)
	bge	1f
	fsub.l	#-2147483648,fp0
	fsub.l	#-2147483648,fp0
1:
	fmove.d	fp0,(sp)
	jmp	(a0)

.cfi:
	move.l	(sp)+,a0
	move.w	(sp),d1
	move.w	2(sp),d0
	cmp.w	#4,d0
	bne	1f
	fmove.s	4(sp),fp0
	bra	2f
1:
	fmove.d	4(sp),fp0
	add.l	#4,sp
2:
	fintrz	fp0,fp0
	cmp.w	#2,d1
	bne	1f
	fmove.w	fp0,6(sp)
	bra	2f
1:
	fmove.l	fp0,4(sp)
2:
	cmp.w	#4,d0
	beq	1f
	sub.l	#4,sp
1:
	jmp	(a0)

.cfu:
	move.l	(sp)+,a0
	move.w	(sp),d1
	move.w	2(sp),d2
	cmp.w	#4,d2
	bne	1f
	fmove.s	4(sp),fp0
	fabs	fp0
	cmp.l	#0x4f000000,4(sp)
	bge	2f
	fintrz	fp0,fp0
	fmove.l	fp0,d0
	bra	3f
2:
	fadd.l	#-2147483648,fp0
	fintrz	fp0,fp0
	fmove.l	fp0,d0
	bchg	#31,d0
	bra	3f
1:
	fmove.d	4(sp),fp0
	add.l	#4,sp
	fabs	fp0
	cmp.l	#0x41e00000,(sp)
	bge	1f
	fintrz	fp0,fp0
	fmove.l	fp0,d0
	bra	3f
1:
	fadd.l	#-2147483648,fp0
	fintrz	fp0,fp0
	fmove.l	fp0,d0
	bchg	#31,d0
3:
	cmp.w	#2,d1
	bne	1f
	move.w	d0,6(sp)
	bra	2f
1:
	move.l	d0,4(sp)
2:
	cmp.w	#4,d2
	beq	1f
	sub.l	#4,sp
1:
	jmp	(a0)

.cff4:
	move.l	(sp)+,a0
	fmove.d	(sp),fp0
	fmove.s	fp0,4(sp)
	jmp	(a0)

.cff8:
	move.l	(sp)+,a0
	fmove.s	(sp),fp0
	fmove.d	fp0,(sp)
	jmp	(a0)

.cmf4:
	move.l	(sp)+,a0
	clr.l	d0
	fmove.s	(sp),fp0
	fmove.s	4(sp),fp1
	fcmp	fp0,fp1
	fbeq	2f
	fblt	1f
	add.l	#1,d0
	jmp	(a0)
1:
	sub.l	#1,d0
2:
	jmp	(a0)

.cmf8:
	move.l	(sp)+,a0
	clr.l	d0
	fmove.d	(sp),fp0
	fmove.d	8(sp),fp1
	fcmp	fp0,fp1
	fbeq	2f
	fblt	1f
	add.l	#1,d0
	jmp	(a0)
1:
	sub.l	#1,d0
2:
	jmp	(a0)
