#include "em_abs.h"
#include "syscall.h"

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
	movl	$m3,ap
	chmk	(ap)+		# restore default handler
	movl	$m5,ap
	chmk	(ap)+		# get current signal mask
	bicl3	$0x80,r0,m4+6	# and remove the 8th bit
	movl	$m4,ap
	chmk	(ap)+		# and 
	movl	(sp)+,ap
	pushl	tab [ap]
	jsb	.trp
	movl	$m1,ap
	chmk	(ap)+
	ret

	.data
m1:
	.word	SYS_sigvec
	.long	3
	.long	8
	.long	m1a
	.long	0
m3:
	.word	SYS_sigvec
	.long	3
	.long	8
	.long	0
	.long	0
m4:
	.word	SYS_sigsetmask
	.long	1
	.long	0
m5:
	.word	SYS_sigblock
	.long	1
	.long	0
m1a:
	.long	sig8
	.long	0
	.long	0
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
