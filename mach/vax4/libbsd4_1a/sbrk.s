.set	break,17
.globl	_sbrk
.globl	_brk
.globl	_end

.align	1
_sbrk:
	.word	0x0000
	addl3	brk,4(ap),-(sp)
	bcc	1f
	movl	$0xFFFFFFFF,(sp)	# will cause an error (I hope)
1:
	movl	brk,r3
	calls	$1,_brk
	tstl	r0
	blss	1f
	movl	r3,r0
1:
	ret

.align	1
_brk:
	.word	0x0000
	chmk	$break
	bcc 	1f
	jmp 	errmon
1:
	movl	4(ap),brk
	clrl	r0
	ret

.data
brk:	.long	_end
