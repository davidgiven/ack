 # VAX code for the top_* primitives

	.set	BIG, 0x8000	# 32K chunk per movc3
	.text
	.align 1
	.globl _top_size
	.globl _top_save
	.globl _top_load

_top_size: .word 0x0000
	subl3	sp, 4(ap), r0	# bytes between stack pointer and break
	addl2	$(8+6+1)*4, r0	# add 8 regs, 6 pushed longwords (line, file,
	ret			# ap, fp, size, sp) and 1 extra argument

_top_save: .word 0x0ff0		# save regs r4-r11
	movq	hol0, -(sp)	# push line number and file name
	movq	ap, -(sp)	# push LB equivalents ap and fp
	pushl	4(ap)		# push size
	pushal	-4(sp)		# push sp (the sp AFTER pushing)
	movl	$BIG, r6	# chunk size in r6
	movl	4(ap), r7	# size of block to move
	movl	sp, r1		# source address
	movl	8(ap), r3	# destination address
	cmpl	r7, r6
	jlequ	0f
1:	movc3	r6, (r1), (r3)	# move chunk of the block, add r6 to r1 and r3
	subl2	r6, r7
	cmpl	r7, r6
	jgtru	1b
0:	movc3	r7, (r1), (r3)	# move what's left
	movl	$1, r0		# return 1
	ret

_top_load: .word 0x0000
	movl	4(ap), r1	# source
	movl	(r1), sp	# restore sp
	movl	$BIG, r6	# chunk size
	movl	4(r1), r7	# size
	movl	sp, r3		# destination
	cmpl	r7, r6
	jlequ	0f
1:	movc3	r6, (r1), (r3)	# move chunk of the block back
	subl2	r6, r7
	cmpl	r7, r6
	jgtru	1b
0:	movc3	r7, (r1), (r3)	# move what's left back
	addl2	$8, sp		# pop saved sp and size
	movq	(sp)+, ap	# pop LB's
	movq	(sp)+, hol0	# pop line and file
	clrl	r0		# return 0
	ret
