.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	cerror
.define	_errno

.sect .bss
.comm _errno, 4

.sect .text
cerror:
	movl	r0,_errno
	mnegl	$1,r0
	ret
