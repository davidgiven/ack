.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
fork = 2
.define	__fork

__fork:
	.data2	0x0000
	chmk	$fork
	bcc	1f
	jmp	cerror
1:
	blbc	r1,1f
	clrl	r0
1:
	ret
