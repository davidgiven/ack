#include "syscall.h"
DEFINE(_fork)
	chmk	$SYS_fork
	bcs	err
	jlbc	r1,parent
	clrl	r0
parent:
	ret
err:
	jmp	cerror
