.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_vwait
vwait = 71

.align	1
_vwait:
	.data2	0x0000
	chmk	$vwait
	bcc 	1f
	jmp 	errmon
1:
	tstl	4(ap)
	beql	1f
	movl	r1,*4(ap)
1:
	ret
