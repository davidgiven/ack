.sect .text

! Store to bounds-checked array.
!
! Stack: ( element array-adr index descriptor-adr -- )

.define .sar4
.sar4:
	mfspr r10, lr
	bl .aar4
	! pass r3 = size from .aar4 to .sts4
	bl .sts4
	mtspr lr, r10
	blr
