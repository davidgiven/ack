.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
wait = 7
.define	_wait

.align	1
_wait:
	.data2	0x0000
	chmk	$wait
	bcc 	1f
	jmp	errmon
1:
	tstl	4(ap)
	beql	1f
	movl	r1,*4(ap)
1:
	ret
