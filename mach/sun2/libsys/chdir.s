#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _chdir
_chdir: SYSTEM(SYS_chdir)
