/ $Header$
.text
.globl unknown~
.globl fat~

EILLSIZ	= 19.

unknown~:
	mov	$EILLSIZ,-(sp)
	jmp	fat~
