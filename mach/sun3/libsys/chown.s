#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _chown
_chown: SYSTEM(SYS_chown)
