#include "syscall.h"
.globl	_environ
DEFINE(_execv)
	pushl	_environ
	pushl	8(ap)
	pushl	4(ap)
	calls	$3,_execve
	ret
