.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
stime = 25
.define	_stime

_stime:
	.data2	0x0000
	movl	*4(ap),4(ap)
	chmk	$stime
	bcc 	1f
	jmp 	cerror
1:
	clrl	r0
	ret
