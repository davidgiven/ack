.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define PRr2,PR2r2,PR4r2,PR6r2
.define PRr2r4,PR2r2r4,PR4r2r4,PR6r2r4
.define RT,RTr2,RTr2r4
! $Id$

PR6r2:	mov $6,r0;br PRr2
PR4r2:	mov $4,r0;br PRr2
PR2r2:	mov $2,r0
PRr2:	mov r5,r1
	mov sp,r5
	sub r0,sp
	mov r2,-(sp)
	mov r1,pc

PR2r2r4:mov $2,r0;br PRr2r4
PR4r2r4:mov $4,r0;br PRr2r4
PR6r2r4:mov $6,r0
PRr2r4:	mov r5,r1
	mov sp,r5
	sub r0,sp
	mov r2,-(sp)
	mov r4,-(sp)
	mov r1,pc

RTr2r4:
	mov (sp)+,r4
RTr2:
	mov (sp)+,r2
RT:
	mov r5,sp
	mov (sp)+,r5
	rts pc
