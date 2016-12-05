.sect .text
.define _mprotect
_mprotect:
	mov eax, 74
	int 0x80
	jb .set_errno
	ret
