        # $Header$
.globl  .lxl

	# nlevels in r0 (>=2)
.lxl:
	pushl	r0
	decl	(sp)
	movl	4(ap),r0
L1:
	jsb	.lpb
	movl	(r0),r0
	sobgtr	(sp),L1
	tstl	(sp)+
	rsb
