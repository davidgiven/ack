.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define gto~
! $Id$

gto~:
	mov     (sp)+,r4
	mov     4(r4),r5
	mov     2(r4),sp
	mov      (r4),pc
!
!	mov	(sp)+,r3
!1:	cmp	4(r3),r5
!	jeq	2f
!	mov	2(r5),r4
!	mov	4(r5),r2
!	mov	(r5),r5
!	br	1b
!2:	mov	2(r3),sp
!	jmp	*(r3)
