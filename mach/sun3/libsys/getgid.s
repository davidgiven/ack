#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _getgid
_getgid: SYSTEM(SYS_getgid)
