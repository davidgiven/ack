#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _umask
_umask: SYSTEM(SYS_umask)
