.globl	_vfork
.set	vfork,66

.align	1
_vfork:
	.word	0x0000
	movl	16(fp),r2
	movab	1f,16(fp)	# dirty. Only one process can do return,
				# so do it before forking
	ret
1:
	chmk	$vfork
	bcs	3f
	tstl	r1
	bneq	1f
	clrl	r0
1:
	jmp	(r2)
3:
	movl	r0,_errno
	mnegl	$1,r0
	jmp	(r2)
