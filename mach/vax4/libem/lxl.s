.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define  .lxl

        ! $Id$

	! nlevels in r0 (>=2)
.lxl:
	pushl	r0
	decl	(sp)
	movl	4(ap),r0
I1:
	jsb	.lpb
	movl	(r0),r0
	sobgtr	(sp),I1
	tstl	(sp)+
	rsb
