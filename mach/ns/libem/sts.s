.define	.sts
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

	.sect .text

.sts:
	movd tos, r3
	movd tos, r0
	movd tos, r1
	cmpqd 1, r0
	beq 1f
	cmpqd 2, r0
	beq 2f
3:
	movd tos, 0(r1)
	addqd 4, r1
	acbd -4, r0, 3b
4:
	movd r3, tos
	ret 0
1:	
	movb tos, 0(r1)
	adjspd -3
	br 4b
2:
	movw tos, 0(r1)
	adjspd -2
	br 4b
