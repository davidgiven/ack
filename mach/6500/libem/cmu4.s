.define Cmu4

! This subroutine compares two unsigned fourbyte integers.
! If T is first pushed and than S the routine will return:
!    -1   if S < T,
!     0   if S = T,
!     1   if S > T.


Cmu4:
	lda #ARTH
	sta ADDR
	lda #0
	sta ADDR+1
	jsr Sdo		! store S in zeropage ARTH - ARTH+3
	lda #ARTH+4
	sta ADDR
	jsr Sdo		! store T in zeropage ARTH+4 - ARTH+7
	lda ARTH+7
	cmp ARTH+3
	bcc 3f		! S (lowbyte+3) < T (lowbyte+3)
	bne 2f		! S (lowbyte+3) < T (lowbyte+3)
	lda ARTH+6
	cmp ARTH+2
	bcc 3f		! S (lowbyte+2) < T (lowbyte+2)
	bne 2f		! S (lowbyte+2) < T (lowbyte+2)
	lda ARTH+5
	cmp ARTH+1
	bcc 3f		! S (lowbyte+1) < T (lowbyte+1)
	bne 2f		! S (lowbyte+1) < T (lowbyte+1)
	lda ARTH+4
	cmp ARTH
	bcc 3f		! S (lowbyte+0) < T (lowbyte+0)
	bne 2f		! S (lowbyte+0) < T (lowbyte+0)
	lda #0
	tax		! S = T
	rts
    2:	lda #0		! S > T
	ldx #1
	rts
    3:	lda #0FFh	! S < T
	tax
	rts


