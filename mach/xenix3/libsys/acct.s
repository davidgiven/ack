.sect .text; .sect .rom; .sect .data; .sect .bss
.define _acct
.sect .text
_acct:
	mov	ax,51
	jmp	sysal
