        # $Header$
#include "em_abs.h"

.globl .los

.los:
	movl    (sp)+,r2
	cmpl    r0,$4
	beql    L1
	pushl	$EILLINS
	jmp     .fat
L1:
	movl	(sp)+,r0	# nbytes in r0
	movl	(sp)+,r1	# address in r1
	cmpl	r0,$1
	beql	L2
	cmpl	r0,$2
	beql	L3
	addl2	r0,r1
	ashl	$-2,r0,r0
L4:
	movl	-(r1),-(sp)
	sobgtr	r0,L4
	jmp	(r2)
L2:
	cvtbl	(r1),-(sp)
	jmp	(r2)
L3:
	cvtwl	(r1),-(sp)
	jmp	(r2)
