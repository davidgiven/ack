.global $errno, cerror, _cerror

.type _cerror, #function

.section ".text"
cerror:
_cerror:
	set	$errno, %o5
	st	%o0, [%o5]
	set	errno, %o5
	st	%o0, [%o5]
	set	-1, %o0
	retl
	nop

.section ".data"
$errno:
	.long	0
