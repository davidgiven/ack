        # $Header$
#include "em_abs.h"

.globl  .csb4

.csb4:
	movl    (sp)+,r0        # descriptor address
	movl    (sp)+,r1        # index
	pushl	r4		# Save r4
	movl    (r0)+,r4        # default
	movl	(r0)+,r2        # number of cases
L1:
	decl    r2
	blss    L2
	movl    (r0)+,r3
	cmpl    r1,r3
	beql	Lfound
	tstl	(r0)+		# useless address
	brb	L1
Lfound:
	movl    (r0)+,r4
L2:
	movl	r4,r0
	beql    a3
	movl	(sp)+,r4	# Restore r4
	jmp     (r0)
a3:
	pushl	$ECASE
	jmp     .fat
