.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _time
.extern _time, _ftime
_time:
	push	si
	push	di
	push	bp
	mov	bp,sp
	sub	sp,10
	lea	ax,-10(bp)
	push	ax
	call	_ftime
	add	sp,2
	cmp	8(bp),0
	je	1f
	mov	ax,-10(bp)
	mov	dx,-8(bp)
	mov	di,8(bp)
	mov	(di),ax
	mov	2(di),dx
1:	mov	ax,-10(bp)
	mov	dx,-8(bp)
	mov	sp,bp
	pop	bp
	pop	di
	pop	si
	ret
