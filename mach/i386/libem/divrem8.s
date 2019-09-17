.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .divrem8

yl=12
yh=16
xl=20
xh=24
	! This private sub for .dvi8, .dvu8, .rmi8, .rmu8
	! does unsigned division of x = xh:xl by y = yh:yl,
	! yields ebx:eax = quotient, ecx:edx = remainder.

.divrem8:
	! Caller must set eax, edx, flag z like so:
	! mov	edx,yh(esp)
	! test	edx,edx
	! mov	eax,xh(esp)
	jnz	1f		! jump if y >= 2**32

	! y = yl, so x / y = xh:xl / yl = qh:0 + (xl + rh) / yl
	! where qh, rh are quotient, remainder from xh / yl.
	mov	ecx,yl(esp)
	xor	edx,edx		! edx:eax = xh
	div	ecx		! eax = qh, edx = rh
	mov	ebx,eax
	mov	eax,xl(esp)	! edx:eax = qh:xl
	div	ecx		! ebx:eax = qh:ql = quotient
	xor	ecx,ecx		! ecx:edx =  0:rl = remainder
	ret

1:	! Here y >= 2**32.  Find y >> cl in [2**31, 2**32).
	mov	ebx,yl(esp)	! edx:ebx = y
	bsr	ecx,edx		! scan yh for highest set bit
	incb	cl		! cl bits from cl-1 to 0
	shrd	ebx,edx,cl	! ebx = y >> cl

	! Estimate x / y as q = (x / (y >> cl)) >> cl.
	xor	edx,edx		! edx:eax = xh
	div	ebx		! eax = xh / (y >> cl)
	push	eax
	mov	eax,xl+4(esp)	! push moved xl to xl+4
	div	ebx
	pop	edx		! edx:eax = x / (y >> cl)
	shrd	eax,edx,cl	! eax = q

	! Calculate the remainder x - y * q.  If the subtraction
	! overflows, then the correct quotient is q - 1, else it is q.
	mov	ebx,yh(esp)
	imul	ebx,eax		! ebx = yh * q
	push	eax
	mul	yl+4(esp)	! edx:eax = yl * q
	add	ebx,edx		! ebx:eax = y * q
	mov	edx,xl+4(esp)
	mov	ecx,xh+4(esp)
	sub	edx,eax
	sbb	ecx,ebx		! ecx:edx = remainder
	pop	eax		! eax = q
	jnc	1f		! jump unless subtraction overflowed
	dec	eax		! fix quotient
	add	edx,yl(esp)
	adc	ecx,yh(esp)	! fix remainder
1:	xor	ebx,ebx		! ebx:eax = quotient
	ret
