.define	.print
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

	.sect .text

.print:
	save[r0,r1,r2]
	movd 16(sp), r0
	movd 1000000000, r2
	cmpqd 0, r0
	ble 4f
	movb "-", tos
	jsr @.putchar
	negd r0, r0
4:
!	deid r2, r0
	movd r0, r1
	divd r2, r1
	modd r2, r0
	addb "0", r1
	movb r1, tos
	jsr @.putchar
	divd 10, r2
	cmpqd 0, r2
	bne 4b
	restore[r0,r1,r2]
	ret 4
