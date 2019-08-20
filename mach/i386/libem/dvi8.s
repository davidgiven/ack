.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .dvi8, .dvu8

yl=8
yh=12
xl=16
xh=20
	! .dvi8 and .dvu8 divide x = xh:xl by y = yh:yl,
	! yield edx:eax = quotient, ecx:ebx = remainder.

.dvu8:
	! Unsigned division: set di = 0 for non-negative quotient.
	push	edi
	xor	di,di
	mov	eax,xh(esp)
	mov	edx,yh(esp)
	and	edx,edx
	jmp	7f

.dvi8:
	! Signed division: replace x and y with their absolute values.
	! Set di = 1 for negative quotient, 0 for non-negative.
	push	edi
	xor	di,di		! di = 0
	mov	eax,xh(esp)
	and	eax,eax
	jns	1f
	inc	di		! di = 1
	neg	eax
	neg	xl(esp)
	sbb	eax,0		! eax:xl = absolute value of x
1:	mov	edx,yh(esp)
	and	edx,edx
	jns	7f
	xor	di,1		! flip di
	neg	edx
	neg	yl(esp)
	sbb	edx,0		! edx:yl = absolute value of y

7:	! Here .dvu8 joins .dvi8, eax = xh, edx = yh, flags test edx,
	! the values in xh(esp) and yh(esp) are garbage.
	jnz	8f		! jump if y >= 2**32

	! x / y = x / yl = xh / yl + xl / yl = qh + (xl + rh) / yl
	! where qh and rh are quotient, remainder from xh / yl.
	mov	ebx,yl(esp)
	xor	edx,edx		! edx:eax = xh
	div	ebx		! eax = qh, edx = rh
	mov	ecx,eax
	mov	eax,xl(esp)
	div	ebx		! eax = ql, edx = remainder
	mov	ebx,edx
	mov	edx,ecx		! edx:eax = quotient qh:ql
	xor	ecx,ecx		! ecx:ebx = remainder

9:	! Finally, if di != 0 then negate quotient, remainder.
	and	di,di
	jz	1f
	neg	edx
	neg	eax
	sbb	edx,0		! negate quotient edx:eax
	neg	ecx
	neg	ebx
	sbb	ecx,0		! negate remainder ecx:ebx
1:	pop	edi		! caller's edi
	ret	16

8:	! We come here if y >= 2**32.
	mov	xh(esp),eax
	mov	yh(esp),edx
	mov	ebx,yl(esp)	! edx:ebx = y

	! Estimate x / y as q = (x / (y >> cl)) >> cl,
	! where 2**31 <= (y >> cl) < 2**32.
	xor	cx,cx
1:	inc	cx
	shr	edx,1
	rcr	ebx,1		! edx:ebx = y >> cl
	and	edx,edx
	jnz	1b		! loop until y >> cl fits in ebx

	! x / (y >> cl) = qh + (x + rh) / (y >> cl)
	push	edi
	xor	edx,edx		! edx:eax = xh
	div	ebx		! eax = qh, edx = rh
	mov	edi,eax
	mov	eax,xl+4(esp)	! push edi moved xl to xl+4
	div	ebx		! edi:eax = x / (y >> cl)

	! q = (x / (y >> cl)) >> cl = esi:eax >> cl
	shr	eax,cl
	neg	cx		! cl = (32 - cl) modulo 32
	shl	edi,cl
	or	eax,edi		! eax = q

	! Calculate the remainder x - q * y.  If the subtraction
	! overflows, then the correct quotient is q - 1, else it is q.
	mov	ecx,yh+4(esp)
	imul	ecx,eax		! ecx = q * yh
	mov	edi,eax
	mul	yl+4(esp)	! edx:eax = q * yl
	add	edx,ecx		! edx:eax = q * y
	mov	ebx,xl+4(esp)
	mov	ecx,xh+4(esp)	! ecx:ebx = x
	sub	ebx,eax
	sbb	ecx,edx		! ecx:ebx = remainder
	jnc	1f
	dec	edi		! fix quotient
	add	ebx,yl+4(esp)
	adc	ebx,yh+4(esp)	! fix remainder
1:	mov	eax,edi
	xor	edx,edx		! edx:eax = quotient
	pop	edi		! negative flag
	jmp	9b
