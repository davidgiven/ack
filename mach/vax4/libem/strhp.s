#include "em_abs.h"

        # $Header$

.globl  .strhp

.strhp:
	movl    (sp)+,r1
	movl    (sp)+,r0
	pushl	r1
	movl    r0,.reghp
	cmpl    r0,.hplim
	blssu   L2
	addl2   $02000,r0
	bicl2   $0777,r0
	movl    r0,.hplim
	movl	ap,r1
	movl    $L9,ap
	chmk    (ap)+
	bcc     L1
	pushl	$EHEAP
	jsb     .trp
	rsb
L1:
	movl	r1,ap
L2:
	rsb

	.data
L9:     .word   17
	.long   1
.hplim: .long   _end
