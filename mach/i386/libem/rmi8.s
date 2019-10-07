.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .rmi8

yl=4
yh=8
xl=12
xh=16
	! .rmi8 yields ecx:edx = remainder from x / y

.rmi8:
	xorb	cl,cl		! cl = 0, non-negative result
	mov	eax,xh(esp)	! eax for .divrem8
	test	eax,eax
	jge	1f		! jump unless x < 0
	incb	cl		! cl = 1, negative result
	neg	eax
	neg	xl(esp)
	sbb	eax,0
	mov	xh(esp),eax	! x = absolute value
1:	mov	edx,yh(esp)	! edx for .divrem8
	test	edx,edx		! flag z for .divrem8 when y >= 0
	jge	1f		! jump unless y < 0
	neg	edx
	neg	yl(esp)
	sbb	edx,0		! flag z for .divrem8 when y < 0
	mov	yh(esp),edx	! y = absolute value
1:	push	ecx
	call	.divrem8
	pop	eax
	testb	al,al
	jz	1f		! jump unless result < 0
	neg	ecx
	neg	edx
	sbb	ecx,0		! negate remainder ecx:edx
1:	ret	16
