.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	__dup
.define	__dup2
dup = 41

.align	1
__dup:
	.data2	0x0000
	brb	1f

.align	1
__dup2:
	.data2	0x0000
	bisb2	$0100,4(ap)
1:
	chmk	$dup
	bcc 	1f
	jmp 	errmon
1:
	ret
