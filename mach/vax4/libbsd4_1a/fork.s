.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_fork
fork = 2

.align	1
_fork:
	.data2	0x0000
	chmk	$fork
	bcc	1f
	jmp	errmon
1:
	jlbc	r1,1f
	clrl	r0
1:
	ret
