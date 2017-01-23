.sect .text

! Index into a bounds-checked array.
!
! On entry:
!    r3 = ptr to descriptor
!    r4 = index
!    r5 = address of array
! Yields:
!    r3 = address of element
!    r0 = size of element (used by .lar4, .sar4)
! Preserves r10 for .lar4, .sar4

.define .aar4
.aar4:
	lis r0, hi16[.trap_earray]
	ori r0, r0, lo16[.trap_earray]
	mtspr ctr, r0            ! load CTR with trap address

	lwz r0, 0(r3)
	subf. r4, r0, r4         ! adjust range
	bltctr                   ! check lower bound

	lwz r0, 4(r3)
	cmplw r4, r3
	bgectr                   ! check upper bound

	lwz r0, 8(r3)
	mullw r4, r4, r0         ! scale index
	add r3, r4, r5           ! calculate element address

	blr
