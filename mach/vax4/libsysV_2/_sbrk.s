.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
break = 17
.define	__sbrk
.define	__brk

__sbrk:
	.data2	0x0000
	movl	Ind,r0
	tstl	4(ap)
	jeql	3f
	addl3	r0,4(ap),-(sp)
	bcs	1f
	pushl	$1
	movl	ap,r3
	movl	sp,ap
	chmk	$break
	bcc 	2f
1:
	jmp 	cerror
2:
	movl	Ind,r0
	addl2	4(r3),Ind
3:
	ret

__brk:
	.data2	0x0000
	chmk	$break
	bcc 	1f
	jmp 	cerror
1:
	movl	4(ap),Ind
	clrl	r0
	ret

	.sect .data
Ind:	.data4	endbss
