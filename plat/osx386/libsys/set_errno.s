.sect .text
.define .set_errno
.set_errno:
	mov (_errno), eax
	mov eax, -1
	ret
