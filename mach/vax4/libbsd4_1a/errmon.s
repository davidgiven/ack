.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	errmon
.comm	_errno,4

.align	1
errmon:
	movl	r0,_errno
	mnegl	$1,r0
	ret
