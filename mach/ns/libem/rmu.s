.define .rmu

	.text

.rmu:
	enter[r2],0
	movd 1,r2
	andd 12(fp),r2
	movd 12(fp),r0
	lshd -1,r0
	remd 8(fp),r0
	lshd 1,r0
	addd r2,r0
	remd 8(fp), r0
	exit[r2]
	ret 8
