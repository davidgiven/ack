#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define  .csb4

        ! $Id$


.csb4:
	movl    (sp)+,r0        ! descriptor address
	movl    (sp)+,r1        ! index
	pushl	r4		! Save r4
	movl    (r0)+,r4        ! default
	movl	(r0)+,r2        ! number of cases
I1:
	decl    r2
	blss    I2
	movl    (r0)+,r3
	cmpl    r1,r3
	beql	Ifound
	tstl	(r0)+		! useless address
	br	I1
Ifound:
	movl    (r0)+,r4
I2:
	movl	r4,r0
	beql    a3
	movl	(sp)+,r4	! Restore r4
	jmp     (r0)
a3:
	pushl	$ECASE
	jmp     .fat
