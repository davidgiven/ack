.text
.globl gto~

gto~:
;	mov     (sp)+,r4
;	mov     4(r4),r5
;	mov     2(r4),sp
;	jmp     *(r4)

	mov	(sp)+,r3
1:	cmp	4(r3),r5
	jeq	2f
	mov	2(r5),r4
	mov	4(r5),r2
	br	1b
2:	mov	2(r3),sp
	jmp	*(r3)
