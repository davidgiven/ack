.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_time
time = 13

.align	1
_time:
	.data2	0x0000
	chmk	$time
	movl	4(ap),r1
	beql	1f
	movl	r0,(r1)
1:
	ret
