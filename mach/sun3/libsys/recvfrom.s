#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _recvfrom
_recvfrom: SYSTEM(SYS_recvfrom)
