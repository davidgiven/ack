.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .mli4

	! x * y with
	! x.low = si x.high = di
	! y.low = bx y.high = ax

.mli4:
	mul     si              ! xl*yh
	mov     cx,ax
	mov     ax,di
	mul     bx              ! xh*yl
	add     cx,ax           ! xh*yl+xl*yh
	mov     ax,si
	mul     bx              ! xl*yl
	add     dx,cx
	ret
