.define Cmi

! This subroutine compares on two integers.
! If T is pushed first and than S, the routine will return:
!   -1  if S < T,
!    0  if S = T,
!    1  if S > T.


Cmi:
	jsr Sbi2	! subtract operands (T - S)
	bpl 1f		! S >= T
	lda #0FFh	! S < T
	tax		! AX becomes -1
	rts
    1:	beq 2f
    3:	lda #0		! S > T
	ldx #1		! AX becomes 1
	rts
    2:	txa
	bne 3b
	lda #0		! S = T
	tax		! AX becomes 0
	rts


