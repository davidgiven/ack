.define Cmu2

! This subroutine compares two unsigned twobyte integers.
! If T is the first pushed and than S, the routine will return:
!    -1  if S < T,
!     0  if S = T,
!     1  if S > T.

Cmu2:
	stx EXG		! S (lowbyte)
	sta EXG+1	! S (highbyte)
	jsr Pop		! get T
	cmp EXG+1
	beq 2f		! S (highbyte)  =  T (highbyte)
	bcc 1f
    4:	lda #0		! S > T
	ldx #1
	rts
    1:	lda #0FFh	! S < T
	tax
	rts
    2:	cpx EXG
	bne 3f
	lda #0		! S = T
	tax
	rts
    3:	bcc 1b
	bcs 4b


