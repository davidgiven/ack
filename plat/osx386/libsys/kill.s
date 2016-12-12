.sect .text
.define _kill
_kill:
	mov eax, 37
	int 0x80
	jb .set_errno
	ret
