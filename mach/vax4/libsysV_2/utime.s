.set	utime,30
.globl	_utime
.globl	cerror
 
_utime:
	.word	0x0000
	chmk	$utime
	bcc	ok
	jmp	cerror
ok:
	ret
