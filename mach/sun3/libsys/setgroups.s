#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _setgroups
_setgroups: SYSTEM(SYS_setgroups)
