.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .mli8

yl=4
yh=8
	! xl in eax
	! xh in edx

.mli8:
	! x * y = (xh + xl) * (yh + yl)
	!       = xh * yh + xh * yl + xl * yh + xl * yl
	! The term xh * yh overflows to zero.
	mov	ecx,eax
	imul	ecx,yh(esp)	! ecx = xl * yh
	imul	edx,yl(esp)	! edx = xh * yl
	add	ecx,edx
	mul	yl(esp)		! edx:eax = xl * yl
	add	edx,ecx		! edx:eax = x * y
	ret	8
