#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __getpid
__getpid: SYSTEM(SYS_getpid)
