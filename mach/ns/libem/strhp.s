#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define  .strhp

        ! $Id$


.strhp:
	movd    tos,r3
	movd    tos,r0
	movd	r3,tos
	movd    r0,@.reghp
 	cmpd    r0,@.limhp
 	blo	I2
 	addd    02000,r0
 	bicd    0777,r0
 	movd	r0,tos
 	jsr	@BRK
 	adjspd	-4
 	cmpqd	0,r0
 	beq	I2
 	movd	EHEAP,tos
 	jsr     @.trp
I2:
	ret	0
