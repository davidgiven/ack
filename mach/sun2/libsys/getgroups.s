#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _getgroups
_getgroups: SYSTEM(SYS_getgroups)
