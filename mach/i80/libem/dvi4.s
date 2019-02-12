.define .dvi4
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! 32 bits integer divide and remainder routine
! Bit 0 of a-reg is set iff quotient has to be delivered
! Bit 7 of a-reg is set iff the operands are signed, so:
! Expects in a-reg:	0 if called by rmu 4
!			1 if called by dvu 4
!			128 if called by rmi 4
!			129 if called by dvi 4
! Expects on stack:	divisor
!			dividend
! Yields on stack:	quotient or remainder

.dvi4:	pop h
	shld .retadr

	sta .areg
	pop h			! store divisor
	shld block3
	xchg
	pop h
	shld block3+2
	dad d
	jc 1f
	mov a,l
	ora h
	cz eidivz		! trap if divisor = 0

1:	pop h			! store dividend
	shld block1
	pop h
	shld block1+2
	lxi h,0			! store initial value of remainder
	shld block2
	shld block2+2

	mvi b,0
	lda .areg
	ral
	jnc 2f			! jump if unsigned

	lda block1+3
	ral
	jnc 1f
	mvi b,129
	lxi h,block1
	call compl		! dividend is positive now

1:	lda block3+3
	ral
	jnc 2f
	inr b
	lxi h,block3
	call compl		! divisor is positive now

2:	push b			! save b-reg
	mvi b,32

dv0:	lxi h,block1		! left shift: block2 <- block1 <- 0
	mvi c,8
	xra a
1:	mov a,m
	ral
	mov m,a
	inx h
	dcr c
	jnz 1b
	lxi h,block2+3		! which is larger: divisor or remainder?
	lxi d,block3+3
	mvi c,4
1:	ldax d
	cmp m
	jz 0f
	jnc 3f
	jmp 4f
0:	dcx d
	dcx h
	dcr c
	jnz 1b

4:	lxi d,block2		! remainder is larger or equal: subtract divisor
	lxi h,block3
	mvi c,4
	xra a
1:	ldax d
	sbb m
	stax d
	inx d
	inx h
	dcr c
	jnz 1b
	lxi h,block1
	inr m

3:	dcr b
	jnz dv0			! keep looping

	pop b
	lda .areg		! quotient or remainder?
	rar
	jnc 4f

! for dvi 4 and dvu 4 only:
	mov a,b
	rar
	lxi h,block1		! complement quotient if divisor
	cc compl		! and dividend have different signs
	lhld block1+2		! push quotient
	push h
	lhld block1
	push h
	jmp 5f

! for rmi 4 and rmu 4 only:
4:	mov a,b
	ral
	lxi h,block2
	cc compl		! negate remainder if dividend was negative
	lhld block2+2
	push h
	lhld block2
	push h

5:	
	lhld .retadr
	pchl

! make 2's complement of 4 bytes pointed to by hl.
compl:	push b
	mvi c,4
	xra a
1:	mvi a,0
	sbb m
	mov m,a
	inx h
	dcr c
	jnz 1b
	pop b
	ret

