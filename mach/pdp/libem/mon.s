.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
! $Id$
.sect .text
.define mon~

EBADMON = 031

mon~:
	mov	$EBADMON,-(sp)
	jsr	pc,trp~
	tst	(sp)+
	rts	pc
