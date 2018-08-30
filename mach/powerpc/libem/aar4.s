.sect .text

! Get address of element of bounds-checked array.
!
! Stack: ( array-adr index descriptor-adr -- element-adr )
! Sets r3 = size of element for .los4, .sts4
! Preserves r10 for .lar4, .sar4

.define .aar4
.aar4:
	lwz r4, 0(sp)            ! r4 = address of descriptor
	lwz r5, 4(sp)            ! r5 = index
	lwz r6, 8(sp)            ! r6 = address of array

	lwz r0, 0(r4)
	subf. r5, r0, r5         ! subtract lower bound from index
	blt .trap_earray         ! check lower bound

	lwz r0, 4(r4)
	cmplw r5, r0
	bgt .trap_earray         ! check upper bound

	lwz r3, 8(r4)            ! r3 = size of element
	mullw r5, r5, r3         ! scale index by size
	add r6, r6, r5
	stw r6, 8(sp)            ! push address of element
	addi sp, sp, 8
	blr

.trap_earray:
	li r3, 0                 ! EARRAY = 0 in h/em_abs.h
	b .trp
