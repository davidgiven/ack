.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_errno
.define	cerror
cerror:
	movl	r0,_errno
	mnegl	$1,r0
	ret

.sect .bss
.comm _errno, 4
