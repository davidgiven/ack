.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define _execl
.define _execle
.define _execv
.define _execve
_execl:
	jmp __execl
_execle:
	jmp __execle
_execv:
	jmp __execv
_execve:
	jmp __execve
