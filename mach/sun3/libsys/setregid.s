#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _setregid
_setregid: SYSTEM(SYS_setregid)
