.define	.los
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

	.sect .text

.los:
	movd tos, r3
	movd tos, r0
	movd tos, r1
	cmpqd 1, r0
	beq 1f
	cmpqd 2, r0
	beq 2f
	addd r0, r1
3:
	addqd -4, r1
	movd 0(r1), tos
	acbd -4, r0, 3b
4:
	movd r3, tos
	ret 0
1:	
	movzbd 0(r1), tos
	br 4b
2:
	movzwd 0(r1), tos
	br 4b
