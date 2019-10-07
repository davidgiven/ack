.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .dvu8, .rmu8

yl=4
yh=8
xl=12
xh=16
	! .dvu8 yields ebx:eax = quotient from x / y
	! .rmu8 yields ecx:edx = remainder from x / y

.dvu8:
.rmu8:	
	mov	edx,yh(esp)
	test	edx,edx
	mov	eax,xh(esp)	! prepare for .divrem8
	push	ebp		! move esp
	call	.divrem8
	pop	ebp		! move esp
	ret	16
