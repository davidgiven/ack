#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _mkdir
_mkdir: SYSTEM(SYS_mkdir)
