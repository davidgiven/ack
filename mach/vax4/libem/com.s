        # $Header$
.globl  .com

	# bytes in r0
.com:
	movl    (sp)+,r2
	movl    sp,r1
	ashl	$-2,r0,r0
L1:
	mcoml   (r1),(r1)+
	sobgtr  r0,L1
	jmp     (r2)
