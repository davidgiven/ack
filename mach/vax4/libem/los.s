#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .los

        ! $Id$


.los:
	movl    (sp)+,r2
	cmpl    r0,$4
	beql    I1
	pushl	$EILLINS
	jmp     .fat
I1:
	movl	(sp)+,r0	! nbytes in r0
	movl	(sp)+,r1	! address in r1
	cmpl	r0,$1
	beql	I2
	cmpl	r0,$2
	beql	I3
	addl2	r0,r1
	ashl	$-2,r0,r0
I4:
	movl	-(r1),-(sp)
	sobgtr	r0,I4
	jmp	(r2)
I2:
	cvtbl	(r1),-(sp)
	jmp	(r2)
I3:
	cvtwl	(r1),-(sp)
	jmp	(r2)
