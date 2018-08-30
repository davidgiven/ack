.sect .text

! Load from bounds-checked array.
!
! Stack: ( array-adr index descriptor-adr -- element )

.define .lar4
.lar4:
	mfspr r10, lr
	bl .aar4
	! pass r3 = size from .aar4 to .los4
	bl .los4
	mtspr lr, r10
	blr
