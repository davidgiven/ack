#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _setrlimit
_setrlimit: SYSTEM(SYS_setrlimit)
