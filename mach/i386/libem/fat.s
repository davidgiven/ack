.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .fat

.fat:
.extern .trp
.extern EXIT
	call    .trp
	jmp     EXIT
	! no return
