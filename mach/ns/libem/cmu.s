.define	.cmu
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

	.sect .text

.cmu:	movd tos, r3		!return addres
	addr 4(sp), r1
	movd r1, r2
	addd 0(sp), r2
	movd 0(sp), r0
	muld -2, r0
	subd 4, r0
3:
	cmpd 0(r2), 0(r1)
	bhi 1f
	blo 2f
	addr 4(r1), r1
	addr 4(r2), r2
	acbd -4, 0(sp), 3b
	xord r1, r1
4:
	adjspd r0
	movd r3, tos
	ret 0
1:	
	movd 1, r1
	br 4b
2:
	movd -1, r1
	br 4b
