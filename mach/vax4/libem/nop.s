#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define  .nop

        ! $Id$


.nop:
	movl	hol0+LINO_AD,r0
	jsb     printd
	movb	$011,r0
	jsb	printc
	movl	sp,r0
	jsb	printd
	movb    $012,r0
	jmp     printc
