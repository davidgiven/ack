.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define cerror
.extern cerror
.extern _errno
cerror:
	mov (_errno),ax
	mov dx,-1
	mov ax,dx
	ret
