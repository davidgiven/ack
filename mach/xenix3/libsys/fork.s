.sect .text; .sect .rom; .sect .data; .sect .bss
.define _fork, _par_uid
.sect .bss
.comm	_par_uid, 2
.sect .text
_fork:
	mov	ax,2
	call	syscal
	jb	1f
	and	bx,bx
	je	2f
	ret
1:
	mov	(_errno),ax
	mov	ax,-1
	ret
2:
	mov	(_par_uid),ax
	xor	ax,ax
	ret
