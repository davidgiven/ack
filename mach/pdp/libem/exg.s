.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define exg~
! $Id$

exg~:	jsr pc,save~
	mov sp,r4
	sub r0,sp
	mov sp,r3
	mov r0,r1
1:
	mov (r4)+,(r3)+
	sob r0,1b
	asr r1
	mov sp,r4
1:
	mov (r4)+,(r3)+
	sob r1,1b
	mov r4,sp
	jmp retu~

