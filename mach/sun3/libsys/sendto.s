#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _sendto
_sendto: SYSTEM(SYS_sendto)
