#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __setitimer
__setitimer: SYSTEM(SYS_setitimer)
