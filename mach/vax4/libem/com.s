.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define  .com

        ! $Id$

	! bytes in r0
.com:
	movl    (sp)+,r2
	movl    sp,r1
	ashl	$-2,r0,r0
I1:
	mcoml   (r1),(r1)+
	sobgtr  r0,I1
	jmp     (r2)
