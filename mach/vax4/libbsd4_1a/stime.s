.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_stime
stime = 25

.align	1
_stime:
	.data2	0x0000
	movl	*4(ap),4(ap)
	chmk	$stime
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
