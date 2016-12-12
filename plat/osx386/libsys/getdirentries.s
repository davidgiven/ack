.sect .text
.define _getdirentries
_getdirentries:
	mov eax, 196
	int 0x80
	jb .set_errno
	ret
