.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
vadvise = 72
.define	_vadvise

.align	1
_vadvise:
	.data2	0x0000
	chmk	$vadvise
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
