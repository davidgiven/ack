.define .dvi
.sect .text
.sect .rom
.sect .data
.sect .bss

 ! signed long divide
 !-----------------------------------------------------------------------------
 ! rewritten by Kai-Uwe Bloem (i5110401@dbstu1.bitnet) for speed.
 !   #1  01/12/90  initial revision. Minor reduce of shift operations.
 !   #2  03/07/90  use 68000 divu instruction whereever possible. This change
 !		   makes #1 superflous. (derived from my GNU division routine)
 !-----------------------------------------------------------------------------
 ! Some common cases can be handled in a special, much faster way :
 !      1) divisor = 0
 !          => cause trap, then return to user. Result is undefined
 !      2) dividend < divisor
 !          => quotient = 0, remainder = dividend
 !      3) divisor < 0x10000 ( i.e. divisor is only 16 bits wide )
 !          => quotient and remainder can be calculated quite fast by repeated
 !             application of 68000 divu operations (ca. 400 cycles)
 !      4) otherwise (due to #2, #3 dividend, divisor both wider then 16 bits)
 !          => do slow division by shift and subtract
 !-----------------------------------------------------------------------------


 ! register usage:
 ! 	 : d0 divisor
 !         d1 dividend
 ! exit  : d1 quotient
 !         d2 remainder

	.sect .text
.dvi:
	move.l	(sp)+,a1	! return address
	move.l	(sp)+,d0	! divisor
	move.l	(sp)+,d2	! dividend
	move.l	d3,a0		! save d3
	move.l	d4,-(sp)	! save result sign register
	clr.l	d4
	tst.l	d2
	bpl	0f		! dividend is negative ?
	neg.l	d2		! yes - negate
	not.l	d4		! and note negation in d4
0:
	tst.l	d0
	bpl	0f		! divisor is negative ?
	neg.l	d0		! yes - negate
	not.w	d4		! note negation
0:
	clr.l	d1		! prepare quotient
! === case 1: divisor = 0
	tst.l	d0		! divisor = 0 ?
	beq	9f		! yes - divide by zero trap
! === case 2: dividend < divisor
	cmp.l	d0,d2		! dividend < divisor ?
	bcs	8f		! yes - division already finished
! === case 3: divisor <= 0x0ffff
	cmp.l	#0x0ffff,d0	! is divisor only 16 bits wide ?
	bhi	2f
	move.w	d2,d3		! save dividend.l
	clr.w	d2		! prepare dividend.h for divu operation
	swap	d2
	beq	0f		! dividend.h is all zero, no divu necessary
	divu	d0,d2
0:	move.w	d2,d1		! save quotient.h
	swap	d1
	move.w	d3,d2		! divide dividend.l
	divu	d0,d2		! (d2.h = remainder of prev divu)
	move.w	d2,d1		! save qoutient.l
	clr.w	d2		! get remainder
	swap	d2
	bra	8f
! === case 4: divisor and dividend both > 0x0ffff
2:
	move	#32-1,d3	! loop count
4:
	lsl.l	#1,d2		! shift dividend ...
	roxl.l	#1,d1		!  ... into d1
	cmp.l	d0,d1		! compare with divisor
	bcs	5f
	sub.l	d0,d1		! bigger, subtract divisor
	add	#1,d2		! note subtraction in result
5:
	dbra	d3,4b
	exg	d1,d2		! get results in the correct registers
8:
	tst.w	d4		! quotient < 0 ?
	bpl	0f
	neg.l	d1		! yes - negate
0:	tst.l	d4		! remainder < 0 ?
	bpl	0f
	neg.l	d2
0:	move.l	(sp)+,d4	! restore d4
	move.l	a0,d3		! restore d3
	jmp	(a1)

EIDIVZ	= 6
9:	move.w	#EIDIVZ,-(sp)
	jsr	.trp
