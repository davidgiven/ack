#include "syscall.h"
DEFINE(__fork)
	chmk	$SYS_fork
	bcs	Ierr
	blbc	r1,Iparent
	clrl	r0
Iparent:
	ret
Ierr:
	jmp	cerror
