.define .adf4, .adf8, .sbf4, .sbf8, .mlf4, .mlf8, .dvf4, .dvf8
.define .ngf4, .ngf8, .fif4, .fif8, .fef4, .fef8
.define .cif4, .cif8, .cuf4, .cuf8, .cfi, .cfu, .cff4, .cff8
.define .cmf4, .cmf8
.sect .text
.sect .rom
.sect .data
.sect .bss

!	$Id$

!	Implement interface to floating point package for M68881

	.sect .text
.adf4:
	move.l	(sp)+,a0
	fmove.s (sp),fp0
	fadd.s	(4,sp),fp0
	fmove.s	fp0,(4,sp)
	jmp	(a0)

.adf8:
	move.l	(sp)+,a0
	fmove.d (sp),fp0
	fadd.d	(8,sp),fp0
	fmove.d	fp0,(8,sp)
	jmp	(a0)

.sbf4:
	move.l	(sp)+,a0
	fmove.s (sp),fp0
	fmove.s	(4,sp),fp1
	fsub	fp0,fp1
	fmove.s	fp1,(4,sp)
	jmp	(a0)

.sbf8:
	move.l	(sp)+,a0
	fmove.d (sp),fp0
	fmove.d	(8,sp),fp1
	fsub	fp0,fp1
	fmove.d	fp1,(8,sp)
	jmp	(a0)

.mlf4:
	move.l	(sp)+,a0
	fmove.s (sp),fp0
	fmul.s	(4,sp),fp0
	fmove.s	fp0,(4,sp)
	jmp	(a0)

.mlf8:
	move.l	(sp)+,a0
	fmove.d (sp),fp0
	fmul.d	(8,sp),fp0
	fmove.d	fp0,(8,sp)
	jmp	(a0)

.dvf4:
	move.l	(sp)+,a0
	fmove.s (sp),fp0
	fmove.s	(4,sp),fp1
	fdiv	fp0,fp1
	fmove.s	fp1,(4,sp)
	jmp	(a0)

.dvf8:
	move.l	(sp)+,a0
	fmove.d (sp),fp0
	fmove.d	(8,sp),fp1
	fdiv	fp0,fp1
	fmove.d	fp1,(8,sp)
	jmp	(a0)

.ngf4:
	fmove.s	(4,sp),fp0
	fneg	fp0
	fmove.s	fp0,(4,sp)
	rts

.ngf8:
	fmove.d	(4,sp),fp0
	fneg	fp0
	fmove.d	fp0,(4,sp)
	rts

.fif4:
	move.l	(sp)+,a0
	move.l	(sp),a1
	fmove.s	(4,sp),fp0
	fmove.s	(8,sp),fp1
	fmul	fp0,fp1
	fintrz	fp1,fp0
	fsub	fp0,fp1
	fmove.s	fp1,(4,a1)
	fmove.s	fp0,(a1)
	jmp	(a0)

.fif8:
	move.l	(sp)+,a0
	move.l	(sp),a1
	fmove.d	(4,sp),fp0
	fmove.d	(12,sp),fp1
	fmul	fp0,fp1
	fintrz	fp1,fp0
	fsub	fp0,fp1
	fmove.d	fp1,(8,a1)
	fmove.d	fp0,(a1)
	jmp	(a0)

.fef4:
	move.l	(sp)+,a0
	move.l	(sp),a1
	fmove.s	(4,sp),fp0
	fgetexp	fp0,fp1
	fmove.l	fpsr,d0
	and.l	#0x2000,d0	! set if Infinity
	beq	1f
	move.l	#129,(a1)
	fmove.s	(4,sp),fp0
	fblt	2f
	move.l	#0x3f000000,(4,a1)
	jmp	(a0)
2:
	move.l	#0xbf000000,(4,a1)
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
	fmove.s	fp0,(4,a1)
	move.l	d0,(a1)
	jmp	(a0)

.fef8:
	move.l	(sp)+,a0
	move.l	(sp),a1
	fmove.d	(4,sp),fp0
	fgetexp	fp0,fp1
	fmove.l	fpsr,d0
	and.l	#0x2000,d0	! set if Infinity
	beq	1f
	move.l	#1025,(a1)
	fmove.d	(4,sp),fp0
	fblt	2f
	move.l	#0x3fe00000,(4,a1)
	clr.l	(8,a1)
	jmp	(a0)
2:
	move.l	#0xbfe00000,(4,a1)
	clr.l	(8,a1)
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
	fmove.d	fp0,(4,a1)
	move.l	d0,(a1)
	jmp	(a0)

.cif4:
	move.l	(sp)+,a0
	fmove.l	(4,sp),fp0
	fmove.s	fp0,(4,sp)
	jmp	(a0)

.cif8:
	move.l	(sp)+,a0
	fmove.l	(4,sp),fp0
	fmove.d	fp0,(sp)
	jmp	(a0)

.cuf4:
	move.l	(sp)+,a0
	fmove.l	(4,sp),fp0
	tst.l	(4,sp)
	bge	1f
	fsub.l	#-2147483648,fp0
	fsub.l	#-2147483648,fp0
1:
	fmove.s	fp0,(4,sp)
	jmp	(a0)

.cuf8:
	move.l	(sp)+,a0
	fmove.l	(4,sp),fp0
	tst.l	(4,sp)
	bge	1f
	fsub.l	#-2147483648,fp0
	fsub.l	#-2147483648,fp0
1:
	fmove.d	fp0,(sp)
	jmp	(a0)

.cfi:
	move.l	(sp)+,a0
	move.l	(4,sp),d0
	cmp.l	#4,d0
	bne	1f
	fmove.s	(8,sp),fp0
	fintrz	fp0,fp0
	fmove.l	fp0,(8,sp)
	jmp	(a0)
1:
	fmove.d	(8,sp),fp0
	fintrz	fp0,fp0
	fmove.l	fp0,(12,sp)
	jmp	(a0)

.cfu:
	move.l	(sp)+,a0
	move.l	(4,sp),d0
	cmp.l	#4,d0
	bne	1f
	fmove.s	(8,sp),fp0
	fabs	fp0
	cmp.l	#0x4f000000,(8,sp)
	bge	2f
	fintrz	fp0,fp0
	fmove.l	fp0,(8,sp)
	jmp	(a0)
2:
	fadd.l	#-2147483648,fp0
	fintrz	fp0,fp0
	fmove.l	fp0,d0
	bchg	#31,d0
	move.l	d0,(8,sp)
	jmp	(a0)
1:
	fmove.d	(8,sp),fp0
	fabs	fp0
	cmp.l	#0x41e00000,(8,sp)
	bge	1f
	fintrz	fp0,fp0
	fmove.l	fp0,(12,sp)
	jmp	(a0)
1:
	fadd.l	#-2147483648,fp0
	fintrz	fp0,fp0
	fmove.l	fp0,d0
	bchg	#31,d0
	move.l	d0,(12,sp)
	jmp	(a0)

.cff4:
	move.l	(sp)+,a0
	fmove.d	(sp),fp0
	fmove.s	fp0,(4,sp)
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
	fmove.s	(4,sp),fp1
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
	fmove.d	(8,sp),fp1
	fcmp	fp0,fp1
	fbeq	2f
	fblt	1f
	add.l	#1,d0
	jmp	(a0)
1:
	sub.l	#1,d0
2:
	jmp	(a0)
