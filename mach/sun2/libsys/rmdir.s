#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _rmdir
_rmdir: SYSTEM(SYS_rmdir)
