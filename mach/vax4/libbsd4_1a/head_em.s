#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text

	! $Header$

.define	hol0
.define	.reghp
.define	.limhp
.define	.trppc
.define	.trpim
.define EXIT, BRK, WRITE

	! run time startoff
	.data2	0
	bispsw	$0100		! set FU(0100)
	movl	4(sp),r0
	clrl	-4(r0)
	movl	sp,r0
	movl	(r0)+,r1
	movl	r0,r2
1:
	tstl	(r0)+
	bneq	1b
	cmpl	r0,(r2)
	blssu	2f
	tstl	-(r0)
2:
	pushl	r0
	pushl	r2
	pushl	r1
	calls	$3,__m_a_i_n
EXIT:
	movl	$Im2,ap
	movl	r0,6(ap)
	chmk	(ap)+
	halt

break = 17

.align	1
BRK:
	.data2	0x0000
	chmk	$break
	bcc 	1f
	jmp 	errmon
1:
	movl	4(ap),.limhp
	clrl	r0
	ret

write = 4

.align	1
WRITE:
	.data2	0x0000
	chmk	$write
	bcc	1f
	jmp	errmon
1:
	ret

	.sect .data
Im2:
	.data2	1
	.data4	1
	.data4	0
	.data2	0
.reghp:
	.data4	__end
.limhp:
	.data4	__end
hol0:
	.space	8
.trppc:
	.space	4
.trpim:
	.data4	0
