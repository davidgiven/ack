.define	.exg
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

	.sect .text

.exg:
	addr 8(sp), r3
	movd r3, r2
	addd r0, r3
1:
	movd 0(r2), r1
	movd 0(r3), 0(r2)
	movd r1, 0(r3)
	addr 4(r2), r2
	addr 4(r3), r3
	acbd -4, r0, 1b
	ret 0
