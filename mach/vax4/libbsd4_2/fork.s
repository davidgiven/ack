#include "syscall.h"
DEFINE(_fork)
	chmk	$SYS_fork
	bcs	Ierr
	blbc	r1,Iparent
	clrl	r0
Iparent:
	ret
Ierr:
	jmp	cerror
