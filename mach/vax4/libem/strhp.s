#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define  .strhp

        ! $Id$


.strhp:
	movl    (sp)+,r1
	movl    (sp)+,r0
	pushl	r1
	movl    r0,.reghp
	cmpl    r0,.limhp
	blssu   I2
	addl2   $02000,r0
	bicl2   $0777,r0
	pushl	r0
	calls	$1,BRK
	tstl	r0
	beql	I2
	pushl	$EHEAP
	jsb     .trp
	rsb
I2:
	rsb
