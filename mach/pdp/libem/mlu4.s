.text
.globl mlu4~
.globl save~,retu~

mlu4~:
	jsr	pc,save~
	clr	r0
	mov	02(sp),r1
	mov	06(sp),r3
	mul	r3,r0
	tst	r3
	bge	1f
	ashc	$15.,r0
1:	mov	02(sp),r3
	clr	r2
	mul	04(sp),r2
	add	r3,r0
	mov	06(sp),r3
	clr	r2
	mul	(sp),r2
	add	r3,r0
	add	$010,sp
	jmp	retu~
