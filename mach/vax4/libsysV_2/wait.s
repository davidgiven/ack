.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
wait = 7
.define	_wait

_wait:
	.data2	0x0000
	chmk	$wait
	bcc 	1f
	jmp 	cerror
1:
	tstl	4(ap)
	beql	1f
	movl	r1,*4(ap)
1:
	ret
