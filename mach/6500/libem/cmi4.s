.define Cmi4

! This subroutine compares on fourbyte integers.
! If T is pushed first and than S, the routine will return:
!    -1   if S < T,
!     0   if S = T,
!     1   if S > T.


Cmi4:
	jsr Sbi4	! subtract operands (T - S)
	jsr Pop		! get result (lowbyte, lowbyte+1)
	stx ARTH	! store lowbyte
	sta ARTH+1	! store lowbyte+1
	jsr Pop		! get result (lowbyte+2, lowbyte+3)
	tay		! test lowbyte+3
	bpl 1f		! S >= T
	lda #0FFh	! S < T
	tax		! AX becomes -1
	rts
    1:	cmp #0		! test lowbyte+3 on zero
	bne 2f
	cpx #0		! test lowbyte+2 on zero
	bne 2f
	lda #0
	cmp ARTH+1	! test lowbyte+1 on zero
	bne 2f
	cmp ARTH	! test lowbyte on zero
	bne 2f
	lda #0		! S = T
	tax		! AX becomes 0
	rts
    2:	lda #0		! S > T
	ldx #1		! AX becomes 1
	rts


