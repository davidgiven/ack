.set	break,17
.globl	_sbrk
.globl	_brk
.globl	_end
.globl	cerror

_sbrk:
	.word	0x0000
	movl	nd,r0
	tstl	4(ap)
	jeql	out
	addl3	r0,4(ap),-(sp)
	bcs	ovfl
	pushl	$1
	movl	ap,r3
	movl	sp,ap
	chmk	$break
	bcc 	ok
ovfl:
	jmp 	cerror
ok:
	movl	nd,r0
	addl2	4(r3),nd
out:
	ret

.globl	_brk

_brk:
	.word	0x0000
	chmk	$break
	bcc 	ok2
	jmp 	cerror
ok2:
	movl	4(ap),nd
	clrl	r0
	ret

	.data
nd:	.long	_end
