#include "syscall.h"
DEFINE(_vfork)
	movl	16(fp),r2
	movab	newret,16(fp)
	ret			# cleans stack
newret:
	chmk	$SYS_vfork
	bcs	err
	tstl	r1
	beql	parent
	clrl	r0
parent:
	jmp	(r2)
err:
	movl	r0,_errno
	mnegl	$1,r0
	jmp	(r2)
