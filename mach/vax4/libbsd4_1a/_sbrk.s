.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
break = 17
.define	__sbrk
.define	__brk

.align	1
__sbrk:
	.data2	0x0000
	addl3	Ibrk,4(ap),-(sp)
	bcc	1f
	movl	$0xFFFFFFFF,(sp)	! will cause an error (I hope)
1:
	movl	Ibrk,r3
	calls	$1,_brk
	tstl	r0
	blss	1f
	movl	r3,r0
1:
	ret

.align	1
__brk:
	.data2	0x0000
	chmk	$break
	bcc 	1f
	jmp 	errmon
1:
	movl	4(ap),Ibrk
	clrl	r0
	ret

.sect .data
Ibrk:	.data4	endbss
