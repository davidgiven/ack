.global _errno
.global cerror

cerror:
	set	_errno, %o5
	st	%o0, [%o5]
	set	-1, %o0
	retl
	nop

.seg	"data"
_errno:
	.long	0

