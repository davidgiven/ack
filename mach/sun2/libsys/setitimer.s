#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _setitimer
_setitimer: SYSTEM(SYS_setitimer)
