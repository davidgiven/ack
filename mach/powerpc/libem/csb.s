.sect .text

! this is not a subroutine, but just a
! piece of code that computes the jump-
! address and jumps to it.
! traps if resulting address is zero
!
! Stack: ( value tableaddr -- )

.define .csb
.csb:
	lwz r3, 0(sp)
	lwz r4, 4(sp)
	addi sp, sp, 8

	lwz r5, 0(r3)            ! r5 = default target

	lwz r6, 4(r3)            ! fetch count
	mr. r6, r6               ! skip loop if count is zero
	beq 3f                   !   (needed by Modula-2 "CASE i OF END")
	mtspr ctr, r6
1:	lwzu r7, 8(r3)           ! fetch target index, increment pointer
	cmpw r4, r7              ! compare with value
	beq 2f
	bdnz 1b                  ! if not equal, go again
	b 3f

2:	lwz r5, 4(r3)            ! r5 = new target
3:	mtspr ctr, r5
	mr. r5, r5               ! test target
	bnectr                   ! jump to target if non-zero
	b .trap_ecase            ! otherwise trap
