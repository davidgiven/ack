.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define aar~
! $Id$

!r0 : descriptor address
!r1 : element number
!base address is on stack
aar~:
	sub	(r0),r1
	mul	04(r0),r1
	add	r1,02(sp)
	rts	pc
