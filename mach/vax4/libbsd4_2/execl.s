#include "syscall.h"
.extern	_environ
DEFINE(_execl)
	pushl	_environ
	pushab	8(ap)
	pushl	4(ap)
	calls	$3,_execve
	ret
