.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_errno
.define	cerror
.comm _errno, 4
cerror:
	movl	r0,_errno
	mnegl	$1,r0
	ret
