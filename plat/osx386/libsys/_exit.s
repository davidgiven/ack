.sect .text
.define __exit
__exit:
	mov eax, 1
	int 0x80
