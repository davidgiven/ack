#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text

	! $Header$

.define	hol0
.define	.reghp
.define	.limhp
.define	.trppc
.define	.trpim

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
	movl	$Im1,ap
	chmk	(ap)+		! catch floating point exception
	calls	$3,_m_a_i_n
	movl	$Im2,ap
	movl	r0,6(ap)
	chmk	(ap)+
	halt

	.align	1
Isig8:
	.data2	0x0000
	pushl	8(ap)
	movl	(sp)+,ap
	pushl	Itab [ap]
	jsb	.trp
	movl	$Im1,ap
	chmk	(ap)+
	ret

	.sect .rom
Im1:
	.data2	48
	.data4	2
	.data4	8
	.data4	Isig8

Itab:
	.data4	0
	.data4	EIOVFL
	.data4	EIDIVZ
	.data4	EFOVFL
	.data4	EFDIVZ
	.data4	EFUNFL
	.data4	EILLINS
	.data4	EARRAY
	.data4	EFOVFL
	.data4	EFDIVZ
	.data4	EFUNFL

	.sect .data
Im2:
	.data2	1
	.data4	1
	.data4	0
.reghp:
	.data4	_end
.limhp:
	.data4	_end
hol0:
	.space	8
.trppc:
	.space	4
.trpim:
	.data4	0
