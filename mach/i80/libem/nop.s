.define .nop

.nop:	push b
	lhld hol0+4
	mov d,h
	mov e,l
	call prstring
	lxi d,lin
	call prstring
	lhld hol0
	call prdec
	lxi d,stpr
	call prstring
	lxi h,0
	dad sp
	call prdec
	lxi d,newline
	call prstring
	pop b
	ret


lin:	.asciz "   lin:"
stpr:	.asciz "   sp:"
newline:.asciz "\n"
