.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define cerror,_errno
cerror:
	move.l	d0,(_errno)
	move.l	#-1,d0
	rts

.sect .bss
_errno:	.space 4
