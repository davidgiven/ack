.define	.dvu
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

	.sect .text

.dvu:	
	enter[r1,r2],0
	movd 1,r2
	andd 12(fp), r2
	movd 12(fp), r0
	lshd -1,r0
	quod 8(fp), r0
	lshd 1,r0
	movd 12(fp), r1
	lshd -1,r1
	remd 8(fp), r1
	lshd 1,r1
	addd r2, r1
	quod 8(fp),r1
	addd r1, r0
	exit[r1,r2]
	ret 8
