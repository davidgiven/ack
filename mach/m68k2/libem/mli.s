.define .mli
.sect .text
.sect .rom
.sect .data
.sect .bss

 ! signed long mulitply
 !-----------------------------------------------------------------------------
 ! rewritten by Kai-Uwe Bloem (i5110401@dbstu1.bitnet) for speed.
 !   #1  01/12/90  initial revision
 !-----------------------------------------------------------------------------
 !   3 cases worth to recognize :
 !	1) both the upper word of u and v are zero
 !	    => 1 mult : Low*Low
 !	2) only one of the upper words is zero
 !	    => 2 mult : Low*HighLow
 !	3) both upper words are not zero
 !	    => 4 mult : HighLow*HighLow
 !   there are other cases (e.g. lower word is zero but high word is not, or
 !   one operand is all zero). However, this seems not to be very common, so
 !   they are ignored for the price of superfluous multiplications in these
 !   cases.
 !-----------------------------------------------------------------------------

 ! entry : d0 multiplicand
 !         d1 multiplier
 ! exit  : d0 high order result
 !         d1 low order result
 !         d2,a0,a1 : destroyed

	.sect .text
.mli:
	move.l	(sp)+,a1	! return address
	move.l	d3,a0		! save register
	movem.w	(sp)+,d0-d3	! get v and u
	move.w	d5,-(sp)	! save sign register
	move.w	d2,d5
	bge	0f		! negate u if neccessary
	neg.w	d1
	negx.w	d0
0:	tst.w	d0
	bge	0f		! negate v if neccessary
	eor.w	d0,d5
	neg.w	d1
	negx.w	d0
0:	bne	1f		! case 2) or 3)
	tst.w	d2
	bne	2f		! case 2)
! === case 1: _l x _l ===
	mulu	d3,d1		! r.l = u.l x v.l
9:				! (r.h is already zero)
	tst.w	d5		! negate result if neccessary
	bpl	0f
	neg.l	d1
	negx.l	d0
0:	move.w	(sp)+,d5	! return
	move.l	a0,d3
	jmp	(a1)
! === possibly case 2) or case 3) ===
1:
	tst.w	d2
	bne	3f		! case 3)
! === case 2: _l x hl ===
	exg	d0,d2		! exchange u and v
	exg	d1,d3		! (minimizes number of distinct cases)
2:
	mulu	d1,d2		! a = v.l x u.h
	mulu	d3,d1		! r.l = v.l x u.l
	swap	d2		! a = a << 16
	clr.l	d3
	move.w	d2,d3
	clr.w	d2
	add.l	d2,d1		! r += a
	addx.l	d3,d0
	bra	9b
! === case 3: hl x hl ===
3:
	move.l	d4,-(sp)	! need more registers
	move.w	d2,d4
	mulu	d1,d4		! a = v.l x u.h
	mulu	d3,d1		! r.l = u.l x v.l
	mulu	d0,d3		! b = v.h x u.l
	mulu	d2,d0		! r.h = u.h x v.h
	swap	d1		! (just for simplicity)
	add.w	d4,d1		! r += a << 16
	clr.w	d4
	swap	d4
	addx.l	d4,d0
	add.w	d3,d1		! r += b << 16
	clr.w	d3
	swap	d3
	addx.l	d3,d0
	swap	d1
	move.l	(sp)+,d4	! return
	bra	9b
