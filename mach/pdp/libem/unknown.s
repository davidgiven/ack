.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define unknown~
! $Id$
.extern fat~

EILLSIZ	= 023

unknown~:
	mov	$EILLSIZ,-(sp)
	jmp	fat~
