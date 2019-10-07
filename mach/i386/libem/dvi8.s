.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .dvi8

yl=4
yh=8
xl=12
xh=16
	! .dvi8 yields ebx:eax = quotient from x / y

.dvi8:
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
	xorb	cl,1		! flip sign of result
	neg	edx
	neg	yl(esp)
	sbb	edx,0		! flag z for .divrem8 when y < 0
	mov	yh(esp),edx	! y = absolute value
1:	push	ecx
	call	.divrem8
	pop	ecx
	testb	cl,cl
	jz	1f		! jump unless result < 0
	neg	ebx
	neg	eax
	sbb	ebx,0		! negate quotient ebx:eax
1:	ret	16
