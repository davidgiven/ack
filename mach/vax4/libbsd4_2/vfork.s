#include "syscall.h"
DEFINE(_vfork)
	movl	16(fp),r2
	movab	Inewret,16(fp)
	ret			! cleans stack
Inewret:
	chmk	$SYS_vfork
	bcs	Ierr
	tstl	r1
	beql	Iparent
	clrl	r0
Iparent:
	jmp	(r2)
Ierr:
	movl	r0,_errno
	mnegl	$1,r0
	jmp	(r2)
