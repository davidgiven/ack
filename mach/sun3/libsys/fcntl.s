#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _fcntl
_fcntl: SYSTEM(SYS_fcntl)
