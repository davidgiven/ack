#include "SYS.h"

LABEL__(sig_catch)
	save	%sp, %g4, %sp
	mov	%fp, %l0		! get new EM frame pointer

	dec	4, %l0
	st	%i0, [%l0]		! push signal number on EM stack

	set	$_sig_funcs, %l1
	dec	1, %i0
	sll	%i0, 2, %l2
	add	%l1, %l2, %l3
	ld	[%l3], %l2		! get signal handler into %l2

	call	%l2
	nop

	restore
	retl
	add	%sp,%g4,%sp
