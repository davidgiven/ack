.globl	cerror
.globl	_errno

.data
_errno:	.long	0

.text
cerror:
	movl	r0,_errno
	mnegl	$1,r0
	ret
