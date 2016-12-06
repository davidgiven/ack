.sect .text
.define _fstat
_fstat:
	mov eax, 189
	int 0x80
	jb .set_errno
	ret
