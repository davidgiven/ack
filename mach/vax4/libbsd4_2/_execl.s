#include "syscall.h"
.extern	_environ
DEFINE(__execl)
	pushl	_environ
	pushab	8(ap)
	pushl	4(ap)
	calls	$3,__execve
	ret
