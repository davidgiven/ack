#include "em_abs.h"

	# $Header$

.globl	hol0
.globl	.reghp
.globl	.trppc
.globl	.trpim

	# run time startoff
	.word	0
	bispsw	$0100		# set FU(0100)
	movl	4(sp),r0
	clrl	-4(r0)
	movl	sp,r0
	movl	(r0)+,r1
	movl	r0,r2
a1:
	tstl	(r0)+
	bneq	a1
	cmpl	r0,(r2)
	blssu	a2
	tstl	-(r0)
a2:
	pushl	r0
	pushl	r2
	pushl	r1
	movl	$m1,ap
	chmk	(ap)+		# catch floating point exception
	calls	$3,_m_a_i_n
	movl	$m2,ap
	movl	r0,6(ap)
	chmk	(ap)+
	halt

	.align	1
sig8:
	.word	0x0000
	pushl	8(ap)
	movl	(sp)+,ap
	pushl	tab [ap]
	jsb	.trp
	movl	$m1,ap
	chmk	(ap)+
	ret

	.data
m1:
	.word	48
	.long	2
	.long	8
	.long	sig8
m2:
	.word	1
	.long	1
	.long	0
.reghp:
	.long	_end
hol0:
	.space	8
.trppc:
	.space	4
.trpim:
	.long	0
tab:
	.long	0
	.long	EIOVFL
	.long	EIDIVZ
	.long	EFOVFL
	.long	EFDIVZ
	.long	EFUNFL
	.long	EILLINS
	.long	EARRAY
	.long	EFOVFL
	.long	EFDIVZ
	.long	EFUNFL
