#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define  .strhp

        ! $Header$


.strhp:
	movl    (sp)+,r1
	movl    (sp)+,r0
	pushl	r1
	movl    r0,.reghp
	cmpl    r0,.hplim
	blssu   I2
	addl2   $02000,r0
	bicl2   $0777,r0
	movl    r0,.hplim
	movl	ap,r1
	movl    $I9,ap
	chmk    (ap)+
	bcc     I1
	pushl	$EHEAP
	jsb     .trp
	rsb
I1:
	movl	r1,ap
I2:
	rsb

	.sect .data
I9:     .data2   17
	.data4   1
.hplim: .data4   _end
