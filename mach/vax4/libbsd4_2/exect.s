#include "syscall.h"
DEFINE(_exect)
	bispsw	$16
	chmk	$SYS_execve
	jmp	cerror
