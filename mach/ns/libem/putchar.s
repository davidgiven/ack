.define	.putchar
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

	.sect .text

! putchar, for a unix

.putchar:
	enter [],0
	movqd	1,tos
	addr	8(fp),tos
	movqd	1,tos
	bsr	WRITE
	adjspb	-12
	exit	[]
	ret	0
