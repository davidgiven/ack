#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _msgsys
_msgsys: SYSTEM(SYS_msgsys)
