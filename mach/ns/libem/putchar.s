.define	.putchar
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

	.sect .text

.putchar:
	save [r0, r1, r2, r3]
	movqd 4, r0
	addr 20(sp), r1
	movqd 1, r2
	xord r3, r3
	svc
	restore[r0,r1,r2,r3]
	ret 1
