.define .cfi

	.text

.cfi:	cmpd 4, 8(sp)
	beq 4f
	cmpd 8, 8(sp)
	beq 8f
	ret 8
4:
	roundfd 12(sp),12(sp)
	ret 8
8:
	roundld 12(sp), 16(sp)
	ret 12
