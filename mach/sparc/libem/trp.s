.global trp, write

trp:
	set	trpim, %o1
	ld	[%o1], %o2
	srl	%o2, %o0, %o1
	andcc	%o2, 1, %g0
	bz	2f
	nop
	retl
	nop
2:
	set	trppc, %o1
	ld	[%o1], %o1
	tst	%o1
	bz	1f
	nop
	dec	4, %l0
	st	%o0, [%l0]
	jmp	%o1
	nop
1:
	save	%sp, -64, %sp
	set	2, %o0
	set	message, %o1
	set	message_high-message, %o2
	call	write	
	nop
	restore
	ta	2
	st	%g0, [%g0]

message:
	.ascii	"the error code is in %o0\n"
message_high:
