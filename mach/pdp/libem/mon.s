.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
! $Header$
.sect .text
.define mon~

EBADMON = 031

mon~:
	mov	$EBADMON,-(sp)
	jsr	pc,trp~
	tst	(sp)+
	rts	pc
